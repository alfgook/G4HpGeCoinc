
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
// If you want that it inherits the functions 
// implemented in eventAction use instead
// SteppingAction::SteppingAction(EventAction* evAct):
// eventAction(evAct)
{
 
     
}




SteppingAction::~SteppingAction()
{ 
}



void SteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
    
 /////////////////////////////////////////////    
 // Here implement what you want the 
 // simulation to do at every particle 
 // step 
 /////////////////////////////////////////////    
    
}  
