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
	if(aTrack->GetDefinition()==G4Gamma::Definition()) {
		//G4cout << "GetProcessType = " << aTrack->GetCreatorProcess()->GetProcessType() << G4endl;
		if(aTrack->GetCreatorProcess()->GetProcessType()==fDecay) { //
			aTrack->GetVertexKineticEnergy();
			aTrack->GetGlobalTime();
			fEventAction->RegisterPrimaryGamma(aTrack->GetGlobalTime(),aTrack->GetVertexKineticEnergy());
		}
	}
	
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
