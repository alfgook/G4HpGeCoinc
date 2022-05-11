//
/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>
#include "CoincCounterAccumulable.hh"

class G4Run;
class G4GenericMessenger;

/// Run action class

class RunAction : public G4UserRunAction
{
  public:
    RunAction(G4bool master=false);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    void AddNtupleEntry(G4int detectorNbr,G4double Edep, G4double time) {
      vDetectorNbrs.emplace_back(detectorNbr);
      vEnergyDeps.emplace_back(Edep);
      vTimes.emplace_back(time);
    }
    void AddNtupleRow();
    
    
  private:
    G4bool isMaster;
    
    std::vector<G4int> vDetectorNbrs;
    std::vector<G4double> vEnergyDeps;
    std::vector<G4double> vTimes;

    CoincCounterAccumulable *fCoincCounter;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

