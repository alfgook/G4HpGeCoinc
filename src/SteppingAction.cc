
#include "G4ios.hh"
#include "G4SteppingManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4ParticleDefinition.hh"
#include "G4VPhysicalVolume.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4TrackStatus.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <G4UnitsTable.hh>

SteppingAction::SteppingAction(EventAction* evAct)
{
 
}




SteppingAction::~SteppingAction()
{ 
}



void SteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
    G4cout << G4BestUnit(aStep->GetPostStepPoint()->GetGlobalTime(),"Time") << G4endl;

    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
    G4TouchableHistory* theTouchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());
    G4int copyNo = theTouchable->GetVolume()->GetCopyNo();

    G4cout << "copyNo = " << copyNo << G4endl;
}  
