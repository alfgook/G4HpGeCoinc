//
/// \file HPGeHit.cc
/// \brief Implementation of the HPGeHit class

#include "HPGeHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<HPGeHit>* HPGeHitAllocator=0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeHit::HPGeHit() : G4VHit(),
  fVolumeCopyNo(0),
  fParentID(-1),
  fPDGcode(0),
  fEnergyDep(0.),
  fTime(0.),
  fWeight(1.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeHit::~HPGeHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeHit::HPGeHit(const HPGeHit& right) : G4VHit()
{
  fVolumeCopyNo = right.fVolumeCopyNo;
  fParentID   = right.fParentID;
  fPDGcode   = right.fPDGcode;
  fEnergyDep      = right.fEnergyDep;
  fTime      = right.fTime;
  fWeight      = right.fWeight;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeHit::HPGeHit(G4int VolumeCopyNo,
  G4int ParentID,
  G4int PDGcode,
  G4double EnergyDep,
  G4double Time,
  G4double weight = 1.) : G4VHit(),
  fVolumeCopyNo(VolumeCopyNo),
  fParentID(ParentID),
  fPDGcode(PDGcode),
  fEnergyDep(EnergyDep),
  fTime(Time),
  fWeight(weight)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
const HPGeHit& HPGeHit::operator=(const HPGeHit& right)
{
  fVolumeCopyNo = right.fVolumeCopyNo;
  fParentID   = right.fParentID;
  fPDGcode   = right.fPDGcode;
  fEnergyDep      = right.fEnergyDep;
  fTime      = right.fTime;
  fWeight      = right.fWeight;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int HPGeHit::operator==(const HPGeHit& right) const
{
  return ( this == &right ) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HPGeHit::Draw()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HPGeHit::Print()
{
  G4cout << "  PDGcode: " << fPDGcode << " EnergyDeposit: " << G4BestUnit(fEnergyDep,"Energy") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
