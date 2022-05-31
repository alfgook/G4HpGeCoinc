
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

SteppingAction::SteppingAction(EventAction* evAct)
{
 
}




SteppingAction::~SteppingAction()
{ 
}



void SteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
    G4cout << " aStep->GetPreStepPoint()->GetGlobalTime() = " << aStep->GetPreStepPoint()->GetGlobalTime() << G4endl;

    if(aStep->GetTrack()->GetParentID()==1) {

        aStep->GetPostStepPoint()->SetGlobalTime(0);
        aStep->GetPostStepPoint()->SetLocalTime(0);
        aStep->GetPreStepPoint()->SetGlobalTime(0);
        aStep->GetPreStepPoint()->SetLocalTime(0);
        aStep->GetTrack()->SetGlobalTime(0);
    }
}  
