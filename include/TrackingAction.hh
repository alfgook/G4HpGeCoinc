//
//  TrackingAction.h
//  
//
//  Created by Alf Göök on 2022-05-11.
//

#ifndef _TrackingAction_h
#define _TrackingAction_h

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "EventAction.hh"

class TrackingAction : public G4UserTrackingAction
{
    //--------
public:
    //--------
    // Constructor & Destructor
    TrackingAction(EventAction *aEventAction);
    virtual ~TrackingAction();
    // Member functions
    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
    //-----------
private:
    EventAction *fEventAction;
    
protected:
    //-----------
    // Member data
    //G4TrackingManager* fpTrackingManager;
};

#endif