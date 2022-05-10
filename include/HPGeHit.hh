//
// ********************************************************************
// * License and Disclaimer                                           *
//
/// \file HPGeHit.hh
/// \brief Definition of the HPGeHit class

#ifndef HPGeHit_h
#define HPGeHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

/// Tracker hit class
///
/// It defines data members to store the trackID, chamberNb, energy deposit,
/// and position of charged particles in a selected volume:
/// - fTrackID, fChamberNB, fEdep, fPos

class HPGeHit : public G4VHit
{
public:
  HPGeHit();
  HPGeHit(const HPGeHit&);
  HPGeHit(G4int VolumeCopyNo, G4int ParentID, G4int PDGcode, G4double EnergyDep, G4double Time, G4double weight);
  virtual ~HPGeHit();

    // operators
  const HPGeHit& operator=(const HPGeHit&);
  G4int operator==(const HPGeHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

    // methods from base class
  virtual void Draw();
  virtual void Print();

	//
	//HPGeHit *clone(G4int, G4int)

    // Set methods
  void SetPDGcode(G4int aPDGcode) {fPDGcode=aPDGcode;};

    // Get methods
  G4int GetParentID() const     { return fParentID; };
  G4int GetPDGcode() const     { return fPDGcode; };
  G4double GetLight() const     { return fEnergyDep; };
  G4double GetTime() const     { if(fEnergyDep){ return fTime; }else{ return 0.; } };
  G4double GetWeight() const  { return fWeight; };
  G4int GetVolCopyNo() const { return fVolumeCopyNo; };

private:

	G4int				fVolumeCopyNo;
	G4int				fParentID;
	G4int       fPDGcode;
	G4double    fEnergyDep;
	G4double    fTime;
	G4double    fWeight;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<HPGeHit> HPGeHitsCollection;

extern G4ThreadLocal G4Allocator<HPGeHit>* HPGeHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* HPGeHit::operator new(size_t)
{
  if(!HPGeHitAllocator)
    HPGeHitAllocator = new G4Allocator<HPGeHit>;
  return (void *) HPGeHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void HPGeHit::operator delete(void *hit)
{
  HPGeHitAllocator->FreeSingle((HPGeHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif
