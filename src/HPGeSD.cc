/// \file HPGeSD.cc
/// \brief Implementation of the HPGeSD class

#include "HPGeSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeSD::HPGeSD(const G4String& name,  const G4String& HCname) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(HCname);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeSD::~HPGeSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HPGeSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

	//G4cout << "HPGeSD::Initialize()" << G4endl;

  fHitsCollection = new HPGeHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool HPGeSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
	G4int PDGcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

	if(PDGcode!=11 && PDGcode!=22 && PDGcode!=-22 ) {
		return false; // only gammas e- and e+
	}

	G4int trackID = aStep->GetTrack()->GetTrackID();
	G4double trackWeight = aStep->GetTrack()->GetWeight();

	G4TouchableHistory *theTouchable = (G4TouchableHistory*) (aStep->GetPreStepPoint()->GetTouchable());
	G4int VolCopyNo = theTouchable->GetVolume()->GetCopyNo();

	G4double stepTime = aStep->GetPostStepPoint()->GetGlobalTime();
	G4double Edep = aStep->GetTotalEnergyDeposit();
	if(Edep==0.) return true;	

	const G4double detectorResolvingTime = 10.e3; // 10 micro sec

	HPGeHit* theHit = nullptr;
	for(size_t i=0;i<fHitsCollection->entries();i++) {
		//loop over existing hits to find the one for the present physical instance of the detector
		if( ((*fHitsCollection)[i]->GetVolCopyNo() == VolCopyNo)
			  && (fabs((*fHitsCollection)[i]->GetTime() - stepTime) < detectorResolvingTime)) {

			theHit = (*fHitsCollection)[i];

			theHit->SetTime(std::min(theHit->GetTime(),stepTime));
			theHit->AddEdep(Edep);
			// G4cout << "found hit" << G4endl;
			break;
		}
	}
	if(!theHit) {
		theHit = new HPGeHit(VolCopyNo, trackID, PDGcode, Edep, stepTime, trackWeight);
		fHitsCollection->insert( theHit );
	}

	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HPGeSD::EndOfEvent(G4HCofThisEvent*)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

