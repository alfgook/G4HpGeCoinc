//
/// \file DetectorConstruction.hh
/// \brief Definition of the EDDetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4GenericMessenger;
class G4Box;
class HPGeSD;
class G4Material;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();
    void Rebuild();

    G4VPhysicalVolume* SegmentedDetector();
    G4VPhysicalVolume* DualDetector();
    G4VPhysicalVolume* PlanarSegmented();
    G4VPhysicalVolume* SegmentedClover();
    G4VPhysicalVolume* SegmentedClover2();
    G4GenericMessenger  *fMessenger;
    //void SetupDetectors();

  private:
    G4int DetectorType;
    G4int nSegmentsX;
    G4int nSegmentsY;
    G4double sizeX;
    G4double sizeY;
    G4double sizeZ;
    G4double cloverRadius;
    G4double cloverLength;
    G4double cavityRadius;
    G4double cavityDepth;
    G4int nVerticalSegments;


    G4Box *SegmentS;

    std::vector<G4VPhysicalVolume*> SegmentsPV;
    G4LogicalVolume* WorldLV;

    HPGeSD *HPGeDetector;
    G4Material *fGe;
};



#endif

