//
/// \file DetectorConstruction.hh
/// \brief Definition of the EDDetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();
    G4VPhysicalVolume* SegementedDetector();
    G4VPhysicalVolume* DualDetector();
    //void SetupDetectors();
};



#endif

