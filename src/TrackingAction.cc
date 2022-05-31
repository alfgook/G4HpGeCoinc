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
		if(aTrack->GetCreatorProcess()) { // if it is not created by the PrimaryGeneratorAction
			if(aTrack->GetCreatorProcess()->GetProcessType()==fDecay) { // if it was created by a decay process
				fEventAction->RegisterPrimaryGamma(aTrack->GetGlobalTime(),aTrack->GetVertexKineticEnergy());
			}
		}
	}

	if(aTrack->GetParentID()==1) {
		/*G4cout << "-- PreUserTrackingAction ---" << G4endl;
		G4cout << "aTrack->GetGlobalTime() = " << aTrack->GetGlobalTime() << G4endl;*/
		G4cout << "----------------------------" << G4endl;
		((G4Track*) aTrack)->SetGlobalTime(0.);
		((G4Track*) aTrack)->SetLocalTime(0.);
		((G4Track*) aTrack)->SetProperTime(0.);
	}
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
	G4cout << "aTrack->GetGlobalTime() = " << aTrack->GetGlobalTime() << G4endl;
	G4cout << "aTrack->GetLocalTime() = " << aTrack->GetLocalTime() << G4endl;
	if(aTrack->GetParentID()==0) fEventAction->SetTimeOfInitialDecay(aTrack->GetGlobalTime());
}
