//    **********************************
//    *                                *
//    *        EventAction.cc          *
//    *                                *
//    **********************************
#include "EventAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4UImanager.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include <iostream> 
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"
#include "G4AnalysisManager.hh"
#include "G4AccumulableManager.hh"
#include "CoincCounterAccumulable.hh"

#include "HPGeHit.hh"


EventAction::EventAction(RunAction* aRunAction)
{
  hitsCollectionID = -1;
  fRunAction = aRunAction;
  gammas.reserve(100);
}
 
EventAction::~EventAction()
{

    
}
 
void EventAction::BeginOfEventAction(const G4Event* evt)
{ 
  gammas.clear();
}

void EventAction::EndOfEventAction(const G4Event* evt)
{  
  
  //------ analyse the detector data -----------
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  if(hitsCollectionID==-1) {
    hitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("HPGeHC");
    //fSDM->ListTree();
  }

  HPGeHitsCollection* hitsCollection = 
   static_cast<HPGeHitsCollection*>(evt->GetHCofThisEvent()->GetHC(hitsCollectionID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hitsCollectionID; 
    G4Exception("EventAction::EndOfEventAction()",
      "MyCode0003", FatalException, msg);
  }

  const G4double coincTime = 1000.*ns;

  G4double E[2] = {0.,0.};
  for(size_t i=0;i<hitsCollection->entries();++i) { //loop over the hits
    G4int detNbr1 = (*hitsCollection)[i]->GetVolCopyNo();
    G4double time1 = (*hitsCollection)[i]->GetTime();
    G4double Edep1 = (*hitsCollection)[i]->GetEdep()*1000.; // keV

    fRunAction->AddNtupleEntry(detNbr1,Edep1,time1);

    //(*hitsCollection)[i]->Print();
    if(detNbr1==0 || detNbr1==1) {
      analysisManager->FillH1(detNbr1,Edep1);
    }
    for(size_t j=i+1;j<hitsCollection->entries();++j) {
      G4int detNbr2 = (*hitsCollection)[j]->GetVolCopyNo();
      G4double time2 = (*hitsCollection)[j]->GetTime();
      G4double Edep2 = (*hitsCollection)[j]->GetEdep()*1000.; // keV

      if((detNbr1!=detNbr2) && fabs(time1-time2)<coincTime && detNbr1<=1 && detNbr2<=1) {
        E[detNbr1] = Edep1;
        E[detNbr2] = Edep2;
        analysisManager->FillH2(0,E[0],E[1]);
      }
    }
  }
  if(hitsCollection->entries()) fRunAction->AddNtupleRow();

  //------ analyse the source gamma-rays -----------

  for(size_t i=0;i<gammas.size();i++) {
    for(size_t j=i+1;j<gammas.size();j++) {
      if(fabs(gammas[i].first-gammas[j].first)<coincTime) {
        G4int E1(1000.*gammas[i].second+0.5);
        G4int E2(1000.*gammas[j].second+0.5);
        ((CoincCounterAccumulable*) G4AccumulableManager::Instance()->GetAccumulable(0))->CountCoinc(std::make_pair(E1,E2));
      }
    }
  }
}

