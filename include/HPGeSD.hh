/// \file HPGeSD.hh
/// \brief Definition of the HPGeSD class

#ifndef HPGeSD_h
#define HPGeSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4Trajectory.hh"

#include "HPGeHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// HPGe sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class HPGeSD : public G4VSensitiveDetector
{
public:
	HPGeSD(const G4String& name, const G4String& HCname);
	virtual ~HPGeSD();

    // methods from base class
	virtual void   Initialize(G4HCofThisEvent* hitCollection);
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

private:
	HPGeHitsCollection* fHitsCollection;

};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
