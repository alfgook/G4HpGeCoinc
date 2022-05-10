
#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "globals.hh"

class G4Step;
class EventAction;
class G4Event;



 class SteppingAction : public G4UserSteppingAction
{
    
    
public:
  SteppingAction(EventAction*);
  ~SteppingAction();
  void UserSteppingAction(const G4Step* aStep);




private:
  EventAction* eventAction;

};
#endif
