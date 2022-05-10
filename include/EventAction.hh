#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class G4Step;

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  ~EventAction();

public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  
private:
  G4int evtNo;
  G4int hitsCollectionID;

    
};


#endif

    
