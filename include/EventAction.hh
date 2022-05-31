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

  void RegisterPrimaryGamma(G4double time, G4double energy) {
    gammas.emplace_back(std::make_pair(time, energy));
  }

  void SetTimeOfInitialDecay(G4double aTime) {
    fTimeOfInitialDecay = aTime;
  }
  
private:
  G4int evtNo;
  G4int hitsCollectionID;
  RunAction* fRunAction;
  
  std::vector<std::pair<G4double,G4double>> gammas;
  G4double fTimeOfInitialDecay;
};


#endif

    
