#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "HPGeHit.hh"
#include "RunAction.hh"

class G4Step;

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction* aRunAction);
  ~EventAction();

public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  
private:
  G4int evtNo;
  G4int hitsCollectionID;
  RunAction* fRunAction;
    
};


#endif

    
