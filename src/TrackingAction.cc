//
//  TrackingAction.cc
//  
//
//  Created by Alf Göök on 2022-05-11.
//

#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"
#include "G4TrackingManager.hh"
#include "G4Gamma.hh"
#include "G4ProcessType.hh"

TrackingAction::TrackingAction(EventAction *aEventAction)
: G4UserTrackingAction(),
fEventAction(aEventAction)
{

}

TrackingAction::~TrackingAction()
{ 

}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
	if(aTrack->GetDefinition()==G4Gamma::Definition()) { // if the track is a gamma-ray
		// need this else if structure because aTrack->GetCreatorProcess()
		// returns nullptr if the gamma comes from the primary generator
		if(aTrack->GetParentID()==0) {
			fEventAction->RegisterPrimaryGamma(aTrack->GetGlobalTime(),aTrack->GetVertexKineticEnergy());
		} else if (aTrack->GetCreatorProcess()->GetProcessType()==fDecay) {
			fEventAction->RegisterPrimaryGamma(aTrack->GetGlobalTime(),aTrack->GetVertexKineticEnergy());
		} 
	}
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
	
}
