//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1DetectorConstruction.cc 75117 2013-10-28 09:38:37Z gcosmo $
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Isotope.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4PVReplica.hh"
#include <string>
#include "G4GenericMessenger.hh"
#include "G4PVParameterised.hh"
#include "G4Polycone.hh"

//includes for the sensitive detector
#include "HPGeSD.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
# include "G4UnionSolid.hh"
# include "G4Polyhedra.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{
  DetectorType = 0; // to be set by the different construction methods

  nSegmentsX = 3;
  nSegmentsY = 3;

  // parameters of the planar segmented
  sizeX = 80.*mm;
  sizeY = 80.*mm;
  sizeZ = 25.*mm;
  detectorDistance = 50.*mm;

  // parameters of the segmented clover
  cloverRadius = 40.*mm;
  cloverLength = 90.*mm;
  cavityRadius = 7.5*mm;
  cavityDepth = cloverLength*0.9;
  nVerticalSegments = 8;

  //HPGeDetector = nullptr;
  //HPGeDetector = new HPGeSD("HPGeSD","HPGeHC");
  fGe = new G4Material("HPGe",32.,72.640*g/mole,5.323*g/cm3,kStateSolid);

  fMessenger = new G4GenericMessenger(this,"/DetectorConstruction/", "control the detector construction");
  fMessenger->DeclareProperty("detectorType", DetectorType, "type of detector (1 = dual detector, 2 = segmented detector, 3 = planar segmented)");
  fMessenger->DeclareProperty("nSegmentsX", nSegmentsX, "number of segments in X direction");
  fMessenger->DeclareProperty("nSegmentsY", nSegmentsY, "number of segments in Y direction");
  fMessenger->DeclarePropertyWithUnit("SizeX", "mm", sizeX, "size of detector in x direction");
  fMessenger->DeclarePropertyWithUnit("SizeY", "mm", sizeY, "size of detector in y direction");
  fMessenger->DeclarePropertyWithUnit("SizeZ", "mm", sizeZ, "size of detector in z direction (depth)");
  fMessenger->DeclarePropertyWithUnit("DetectorDistance", "mm", detectorDistance, "distance between the two detector planes");

  fMessenger->DeclareProperty("nVerticalSegments", nVerticalSegments, "number of vertical segments");
  fMessenger->DeclarePropertyWithUnit("cloverRadius", "mm", cloverRadius, "radius of circular part of clover");
  fMessenger->DeclarePropertyWithUnit("cloverLength", "mm", cloverLength, "total length of clover");
  fMessenger->DeclarePropertyWithUnit("cavityRadius", "mm", cavityRadius, "radius of the cavity");
  fMessenger->DeclarePropertyWithUnit("cavityDepth", "mm", cavityDepth, "depth of the cavity (must be smaller than length*(1+1/n), where n is the number of vertical segments)");


  fMessenger->DeclareMethod("Rebuild",&DetectorConstruction::Rebuild,
    "Rebuilds the detector").SetStates(G4State_PreInit, G4State_Idle);

}

//Destructor
DetectorConstruction::~DetectorConstruction()
{ }

//Define the detector
G4VPhysicalVolume* DetectorConstruction::Construct()
{  

  G4cout << "DetectorConstruction::Construct()" << G4endl;
  if(DetectorType==2) return SegmentedDetector();
  if(DetectorType==3) return PlanarSegmented();
  if(DetectorType==4) return SegmentedClover();
  if(DetectorType==5) return SegmentedClover2();
  return DualDetector();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{

  static G4ThreadLocal HPGeSD* HPGeDetector = nullptr;
  if(!HPGeDetector) HPGeDetector = new HPGeSD("HPGeSD","HPGeHC");
  G4SDManager::GetSDMpointer()->AddNewDetector(HPGeDetector);

  G4cout << "===================DetectorConstruction::ConstructSDandField()============" << G4endl;
  G4cout << "DetectorType = " << DetectorType << G4endl;
  G4cout << "==========================================================================" << G4endl;

  if(DetectorType==1) {
    //G4LogicalVolume *logicVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DetectorLV");
    //logicVolume->SetSensitiveDetector(HPGeDetector);
    SetSensitiveDetector("DetectorLV",HPGeDetector);
  } else if(DetectorType==2) {
    //G4LogicalVolumeStore::GetInstance()->GetVolume("SmallSegmentLV")->SetSensitiveDetector(HPGeDetector);
    //G4LogicalVolumeStore::GetInstance()->GetVolume("BigSegmentLV")->SetSensitiveDetector(HPGeDetector);
    SetSensitiveDetector("SmallSegmentLV",HPGeDetector);
    SetSensitiveDetector("BigSegmentLV",HPGeDetector);
  }  else if(DetectorType==3) {
    //G4LogicalVolume *logicVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("SegmentLV");
    //logicVolume->SetSensitiveDetector(HPGeDetector);
    SetSensitiveDetector("SegmentLV",HPGeDetector);
  } else if(DetectorType==4 || DetectorType==5) {
    //G4LogicalVolumeStore::GetInstance()->GetVolume("cloverLeafBottomLV")->SetSensitiveDetector(HPGeDetector);
    //G4LogicalVolumeStore::GetInstance()->GetVolume("cloverLeafOtherLV")->SetSensitiveDetector(HPGeDetector);
    SetSensitiveDetector("cloverLeafBottomLV",HPGeDetector);
    SetSensitiveDetector("cloverLeafOtherLV",HPGeDetector);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::Rebuild()
{

  G4cout << "DetectorConstruction::Rebuild()" << G4endl;
  G4RunManager::GetRunManager()->ReinitializeGeometry(true,true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::SegmentedDetector()
{
  DetectorType = 2;
  //============= MATERIAL DEFINITION =================
  G4NistManager* nist = G4NistManager::Instance();  // Get nist material manager
  //G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic"); //Build vacuum material using the nist manager
  //G4Material* BGO = nist->FindOrBuildMaterial("G4_BGO"); //Build vacuum material using the nist manager
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Build vacuum material using the nist manager
  G4Material* fAlu = nist->FindOrBuildMaterial("G4_Al"); //Build vacuum material using the nist manager
  G4Material* fPlexiGlass = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
  G4Material* fCobalt = nist->FindOrBuildMaterial("G4_Co");

  //============= GEOMETRY DESCRIPTION =================

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  
  //========== World ==================================
  G4double world_sizeXY = 50.*cm;
  G4double world_sizeZ  = 50.*cm;

  G4Box* solidWorld =
    new G4Box("WorldSolid",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size


  WorldLV =
    new G4LogicalVolume(solidWorld,          //its solid
                        air,           //its material
                        "WorldLV");            //its name

  G4VPhysicalVolume* WorldPV =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),       //at (0,0,0)
                      WorldLV,            //its logical volume
                      "WorldPV",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // =============== The outer box that houses the HPGe-crystal ===================================================================
  G4double detector_box_height = 120*mm;
  G4double detector_box_widthX = (2.*66.5 + 57.0)*mm;
  G4double detector_box_widthY = 230.0*mm; // measured on drawing
  G4double detector_box_wall_thickness = 10.0*mm;
  G4double detector_box_hole_radius = 0.5*95.0*mm;
  G4double detector_window_thickness = 1.0*mm;

  G4double CrystalSizeX = 57.*mm;
  G4double CrystalSizeY = 54.*mm;
  G4double CrystalSizeZ = 30.*mm;

  // Origo at the center of the crystal in xy-plane, and flush with the top of the detector box in z-direction
  auto CrystalPosition = G4ThreeVector(0,0,26.8*mm+CrystalSizeZ*0.5); 
  auto DetectorBoxPosition = CrystalPosition + G4ThreeVector(0,0.5*detector_box_widthY-49.5*mm - CrystalSizeY*0.5,0.5*detector_box_height-26.8-0.5*CrystalSizeZ); 

  G4Box *DetectorBoxOuterS = new G4Box("DetectorBoxOuterS",0.5*detector_box_widthX,0.5*detector_box_widthY,0.5*detector_box_height);

  G4Box *DetectorBoxInnerS = new G4Box("DetectorBoxOuterS",0.5*(detector_box_widthX - 2.*detector_box_wall_thickness)
                                                          ,0.5*(detector_box_widthY - 2.*detector_box_wall_thickness)
                                                          ,0.5*(detector_box_height - 2.*detector_box_wall_thickness));

  G4SubtractionSolid* DetectorBoxS1 = new G4SubtractionSolid("DetectorBoxS1", DetectorBoxOuterS, DetectorBoxInnerS);

  G4Tubs* DetectorBoxHoleS = new G4Tubs("DetectorBoxHoleS",0.,detector_box_hole_radius,0.5*detector_box_wall_thickness+1.*mm,0.,360.*deg);

  auto HolePosition =  
    -G4ThreeVector(0,0.5*detector_box_widthY - 49.5*mm - CrystalSizeY*0.5,0.5*detector_box_height-0.5*detector_box_wall_thickness);

  G4SubtractionSolid* DetectorBoxS = new G4SubtractionSolid("DetectorBoxS", DetectorBoxS1, DetectorBoxHoleS, 0, HolePosition);

  G4LogicalVolume* DetectorBoxLV = new G4LogicalVolume(DetectorBoxS, fAlu, "DetectorBoxLV");
  new G4PVPlacement(0,                     //no rotation
                      DetectorBoxPosition,
                      DetectorBoxLV,            //its logical volume
                      "DetectorBoxPV",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  auto WindowPosition = DetectorBoxPosition + HolePosition - G4ThreeVector(0,0,0.5*(detector_window_thickness - detector_box_wall_thickness));
  G4Tubs* DetectorWindowS = new G4Tubs("DetectorWindowS",0.,detector_box_hole_radius,0.5*detector_window_thickness,0.,360.*deg);
  G4LogicalVolume* DetectorWindowLV = new G4LogicalVolume(DetectorWindowS, fAlu, "DetectorWindowLV");
  new G4PVPlacement(0,                     //no rotation
                      WindowPosition,
                      DetectorWindowLV,            //its logical volume
                      "DetectorWindowPV",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // =============== The HPGe-crystal ===================================================================
  G4Box *HPGeCrystalS = new G4Box("HPGeCrystalS",0.5*CrystalSizeX,0.5*CrystalSizeY,0.5*CrystalSizeZ);
  G4LogicalVolume* HPGeCrystalLV = new G4LogicalVolume(HPGeCrystalS, fGe, "HPGeCrystalLV");

  new G4PVPlacement(0,                     //no rotation
                      CrystalPosition,
                      HPGeCrystalLV,            //its logical volume
                      "HPGeCrystalPV",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  G4double BigSegmentX = 47.*mm;
  G4double BigSegmentY = 44.*mm;
  G4double BigSegmentZ = CrystalSizeZ;
  G4Box *BigSegmentS = new G4Box("BigSegmentS",0.5*BigSegmentX,0.5*BigSegmentY,0.5*BigSegmentZ);
  G4LogicalVolume* BigSegmentLV = new G4LogicalVolume(BigSegmentS, fGe, "BigSegmentLV");

  new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),
                      BigSegmentLV,            //its logical volume
                      "BigSegmentPV",          //its name
                      HPGeCrystalLV,               //its mother volume
                      false,                 //no boolean operation
                      7,                     //segment number
                      checkOverlaps);        //overlaps checking


  G4double SmallSegmentX = 5.*mm;
  G4double SmallSegmentY = 10.*mm;
  G4double SmallSegmentZ = CrystalSizeZ;
  G4Box *SmallSegmentS = new G4Box("SmallSegmentS",0.5*SmallSegmentX,0.5*SmallSegmentY,0.5*SmallSegmentZ);
  G4LogicalVolume* SmallSegmentLV = new G4LogicalVolume(SmallSegmentS, fGe, "SmallSegmentLV");

  G4String PVname;
  G4double PVposX = 2.5*SmallSegmentX;
  for(int segment=1;segment<7;segment++) {
    PVname = "SmallSegmentPV_" + std::to_string(segment);
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(PVposX,0,0),
                      SmallSegmentLV,            //its logical volume
                      PVname,          //its name
                      BigSegmentLV,               //its mother volume
                      false,                 //no boolean operation
                      segment,                     //segment number
                      checkOverlaps);        //overlaps checking

    PVposX -= SmallSegmentX;
  }

  // =============== The Co-60 source ===================================================================
  G4double SourceContainerRadius = 12.5*mm;
  G4double SourceContainerThickness = 7.0*mm;
  G4double SourceRadius = 3.0*mm;
  G4double SourceThickness = 3.0*mm;

  G4Tubs* SourceContainerS = new G4Tubs("SourceContainerS",0.,SourceContainerRadius,0.5*SourceContainerThickness,0.,360.*deg);
  G4LogicalVolume* SourceContainerLV = new G4LogicalVolume(SourceContainerS, fPlexiGlass, "SourceContainerLV");
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0.5*SourceContainerThickness),
                      SourceContainerLV,            //its logical volume
                      "SourceContainerPV",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  G4Tubs* SourceS = new G4Tubs("SourceS",0.,SourceRadius,0.5*SourceThickness,0.,360.*deg);
  G4LogicalVolume* SourceLV = new G4LogicalVolume(SourceS, fCobalt, "SourceLV");
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0.5*(SourceContainerThickness-SourceThickness)-0.5*mm),
                      SourceLV,            //its logical volume
                      "SourcePV",          //its name
                      SourceContainerLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  // visualization attributes

  // Force the world to be drawn with wireframe
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetForceWireframe(true);
  WorldLV->SetVisAttributes(WorldVisAtt);

  G4VisAttributes *windowVisAtt = new G4VisAttributes();
  windowVisAtt->SetColor(G4Color::Red());
  DetectorWindowLV->SetVisAttributes(windowVisAtt);
  SmallSegmentLV->SetVisAttributes(windowVisAtt);

  G4VisAttributes *HPGeVisAtt = new G4VisAttributes();
  HPGeVisAtt->SetColor(G4Color::Green());
  HPGeCrystalLV->SetVisAttributes(HPGeVisAtt);

  G4VisAttributes *BigSegmentVisAtt = new G4VisAttributes();
  BigSegmentVisAtt->SetColor(G4Color::Blue());
  BigSegmentLV->SetVisAttributes(BigSegmentVisAtt);

  //SetupDetectors();
  return WorldPV; //must return G4VPhysicalVolume pointer to the world

}

G4VPhysicalVolume* DetectorConstruction::DualDetector()
{
  DetectorType = 1;
  //============= MATERIAL DEFINITION =================
  G4NistManager* nist = G4NistManager::Instance();  // Get nist material manager
  //G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic"); //Build vacuum material using the nist manager
  //G4Material* BGO = nist->FindOrBuildMaterial("G4_BGO"); //Build vacuum material using the nist manager
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Build vacuum material using the nist manager
  //G4Material* fAlu = nist->FindOrBuildMaterial("G4_Al"); //Build vacuum material using the nist manager


  //============= GEOMETRY DESCRIPTION =================

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  
  //========== World ==================================
  G4double world_sizeXY = 50.*cm;
  G4double world_sizeZ  = 50.*cm;

  G4Box* solidWorld =
    new G4Box("WorldSolid",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size


  WorldLV =
    new G4LogicalVolume(solidWorld,          //its solid
                        air,           //its material
                        "WorldLV");            //its name

  G4VPhysicalVolume* WorldPV =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),       //at (0,0,0)
                      WorldLV,            //its logical volume
                      "WorldPV",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // Force the detector to be drawn with wireframe
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetForceWireframe(true);
  WorldLV->SetVisAttributes(WorldVisAtt);

  // ========== Cylinder representing a detector crystal =======

  //G4double detector_radius = 2.54*cm; // 1 inch
  //G4double detector_length = 2.54*cm; // 1 inch 

  G4double detector_radius = 45.4864*mm; // Mirion BE6530 
  G4double detector_length = 30.0000*mm; // Mirion BE6530
  //G4double detector_sample_distance = 7.588*cm; // 10 % solid angle coverage
  G4double detector_sample_distance = 25.*mm;

  //G4double detector_radius = 25.0000*mm; // test comparson with Olena's serpent results
  //G4double detector_length = 50.0000*mm; // test comparson with Olena's serpent results
  //G4double detector_sample_distance = 5.00*cm + detector_length*0.5; // test comparson with Olena's serpent results

  // crystal

  G4Tubs* solidDetector = new G4Tubs("DetectorSolid",0.,detector_radius,0.5*detector_length,0.,360.*deg);

  G4LogicalVolume* DetectorLV = 
    new G4LogicalVolume(solidDetector,          //its solid
                        //fLaBr,           //its material
                        fGe,           //its material
                        "DetectorLV");            //its name

  G4RotationMatrix rotDetector;
  rotDetector.rotateX(90*deg);

  /*G4VPhysicalVolume* DetectorPV =*/
    new G4PVPlacement(G4Transform3D(rotDetector,G4ThreeVector(0,detector_sample_distance,0)),
                      DetectorLV,            //its logical volume
                      "DetectorPV1",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  /*G4VPhysicalVolume* DetectorPV =*/
    new G4PVPlacement(G4Transform3D(rotDetector,G4ThreeVector(0,-detector_sample_distance,0)),
                      DetectorLV,            //its logical volume
                      "DetectorPV2",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      1,                     //copy number
                      checkOverlaps);        //overlaps checking
  

  G4VisAttributes *GeVisAtt = new G4VisAttributes(G4Color::Red());
  DetectorLV->SetVisAttributes(GeVisAtt);
  //====================================================

  return WorldPV; //must return G4VPhysicalVolume pointer to the world
}

G4VPhysicalVolume* DetectorConstruction::PlanarSegmented()
{
  DetectorType = 3;
  //============= MATERIAL DEFINITION =================
  G4NistManager* nist = G4NistManager::Instance();  // Get nist material manager
  //G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic"); //Build vacuum material using the nist manager
  //G4Material* BGO = nist->FindOrBuildMaterial("G4_BGO"); //Build vacuum material using the nist manager
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Build vacuum material using the nist manager
  //G4Material* fAlu = nist->FindOrBuildMaterial("G4_Al"); //Build vacuum material using the nist manager


  //============= GEOMETRY DESCRIPTION =================

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  
  //========== World ==================================
  G4double world_sizeXY = 50.*cm;
  G4double world_sizeZ  = 50.*cm;

  G4Box* solidWorld =
    new G4Box("WorldSolid",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size


  WorldLV =
    new G4LogicalVolume(solidWorld,          //its solid
                        air,           //its material
                        "WorldLV");            //its name

  G4VPhysicalVolume* WorldPV =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),       //at (0,0,0)
                      WorldLV,            //its logical volume
                      "WorldPV",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // Force the detector to be drawn with wireframe
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetForceWireframe(true);
  WorldLV->SetVisAttributes(WorldVisAtt);

  // ========== Detector crystal =======

  G4cout << "DetectorConstruction::PlanarSegmented()" << G4endl;
  G4cout << "nSegmentsX = " << nSegmentsX << G4endl;
  G4cout << "nSegmentsY = " << nSegmentsY << G4endl;

  // The full detector volume to be filled with segments (just a dummy volume with air to make it easier to rotate and place the segments)
  G4double minWallThickness = 0.5*mm;
  G4Box *casingS = new G4Box("SegmentS",sizeX*0.5+minWallThickness,sizeY*0.5+minWallThickness,sizeZ*0.5+minWallThickness);
  G4LogicalVolume *casingLV1 = new G4LogicalVolume(casingS,air,"casingLV1");
  G4LogicalVolume *casingLV2 = new G4LogicalVolume(casingS,air,"casingLV2");

  // crystal
  SegmentS = new G4Box("SegmentS",sizeX*0.5/nSegmentsX,sizeY*0.5/nSegmentsY,0.5*sizeZ);
  G4LogicalVolume *SegmentLV = new G4LogicalVolume(SegmentS,fGe,"SegmentLV");

  G4double z0 = 0.5*detectorDistance + 0.5*sizeZ + minWallThickness; //distance from center to each detector plane
  G4double SegmentSizeX = sizeX/nSegmentsX;
  G4double SegmentSizeY = sizeY/nSegmentsY;
  G4int copyno = 0;

  // first plane of detector-segments at - z0
  for(G4int col=0;col<nSegmentsX;col++) {
    G4double x = SegmentSizeX*(col - G4double(nSegmentsX)*0.5 + 0.5);
    for(G4int row=0;row<nSegmentsX;row++) {
      G4double y = SegmentSizeY*(row - G4double(nSegmentsY)*0.5 + 0.5);

      G4String name = "SegmentPV_" + std::to_string(copyno);
      G4VPhysicalVolume *segmentPV = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(x,y,0),       //at (0,0,0)
                      SegmentLV,            //its logical volume
                      name,               //its name
                      casingLV1,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno,                     //copy number
                      checkOverlaps);        //overlaps checking
      ++copyno;
    }
  }

  // second plane of detector-segments at + z0
  for(G4int col=0;col<nSegmentsX;col++) {
    G4double x = SegmentSizeX*(col - G4double(nSegmentsX)*0.5 + 0.5);
    for(G4int row=0;row<nSegmentsX;row++) {
      G4double y = SegmentSizeY*(row - G4double(nSegmentsY)*0.5 + 0.5);

      G4String name = "SegmentPV_" + std::to_string(copyno);
      G4VPhysicalVolume *segmentPV = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(x,y,0),       //at (0,0,0)
                      SegmentLV,            //its logical volume
                      name,               //its name
                      casingLV2,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno,                     //copy number
                      checkOverlaps);        //overlaps checking
      ++copyno;
    }
  }

  G4RotationMatrix rotDetector;
  rotDetector.rotateX(90*deg);

  /*G4VPhysicalVolume *casingPV1 = */
    new G4PVPlacement(G4Transform3D(rotDetector,G4ThreeVector(0,z0,0)),
                      casingLV1,            //its logical volume
                      "casingPV1",               //its name
                      WorldLV,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno,                     //copy number
                      checkOverlaps);        //overlaps checking

  /*G4VPhysicalVolume *casingPV2 = */
    new G4PVPlacement(G4Transform3D(rotDetector,G4ThreeVector(0,-z0,0)),
                      casingLV2,            //its logical volume
                      "casingPV2",               //its name
                      WorldLV,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno,                     //copy number
                      checkOverlaps);        //overlaps checking

/*
  G4double z0 = 0.5*detectorDistance + 0.5*sizeZ
  G4double SegmentSizeX = sizeX/nSegmentsX;
  G4double SegmentSizeY = sizeY/nSegmentsY;
  G4int copyno = 0;

  // first plane of detector-segments at - z0
  for(G4int col=0;col<nSegmentsX;col++) {
    G4double x = SegmentSizeX*(col - G4double(nSegmentsX)*0.5 + 0.5);
    for(G4int row=0;row<nSegmentsX;row++) {
      G4double y = SegmentSizeY*(row - G4double(nSegmentsY)*0.5 + 0.5);

      G4String name = "SegmentPV_" + std::to_string(copyno);
      G4VPhysicalVolume *segmentPV = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(x,y,-z0),       //at (0,0,0)
                      SegmentLV,            //its logical volume
                      name,               //its name
                      WorldLV,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno,                     //copy number
                      checkOverlaps);        //overlaps checking
      SegmentsPV.emplace_back(segmentPV);
      ++copyno;
    }
  }

  // second plane of detector-segments at + z0
  for(G4int col=0;col<nSegmentsX;col++) {
    G4double x = SegmentSizeX*(col - G4double(nSegmentsX)*0.5 + 0.5);
    for(G4int row=0;row<nSegmentsX;row++) {
      G4double y = SegmentSizeY*(row - G4double(nSegmentsY)*0.5 + 0.5);

      G4String name = "SegmentPV_" + std::to_string(copyno);
      G4VPhysicalVolume *segmentPV = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(x,y,z0),       //at (0,0,0)
                      SegmentLV,            //its logical volume
                      name,               //its name
                      WorldLV,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno,                     //copy number
                      checkOverlaps);        //overlaps checking
      SegmentsPV.emplace_back(segmentPV);
      ++copyno;
    }
  }
*/
  //====================================================

  return WorldPV; //must return G4VPhysicalVolume pointer to the world
}


G4VPhysicalVolume* DetectorConstruction::SegmentedClover()
{
  DetectorType = 4;
  //============= MATERIAL DEFINITION =================
  G4NistManager* nist = G4NistManager::Instance();  // Get nist material manager
  //G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic"); //Build vacuum material using the nist manager
  //G4Material* BGO = nist->FindOrBuildMaterial("G4_BGO"); //Build vacuum material using the nist manager
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Build vacuum material using the nist manager
  //G4Material* fAlu = nist->FindOrBuildMaterial("G4_Al"); //Build vacuum material using the nist manager

  G4cout << "****************" << G4endl;
  G4cout << "nVerticalSegments = " << nVerticalSegments << G4endl;
  G4cout << "****************" << G4endl;

  //============= GEOMETRY DESCRIPTION =================

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  
  //========== World ==================================
  G4double world_sizeXY = 50.*cm;
  G4double world_sizeZ  = 50.*cm;

  G4Box* solidWorld =
    new G4Box("WorldSolid",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size


  WorldLV =
    new G4LogicalVolume(solidWorld,          //its solid
                        air,           //its material
                        "WorldLV");            //its name

  G4VPhysicalVolume* WorldPV =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),       //at (0,0,0)
                      WorldLV,            //its logical volume
                      "WorldPV",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // Force the detector to be drawn with wireframe
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetForceWireframe(true);
  WorldLV->SetVisAttributes(WorldVisAtt);

  // ========== Detector crystal =======

  G4double bottomThickness = cloverLength - cavityDepth;
  G4double curCloverLength = cloverLength/nVerticalSegments;
  if(bottomThickness>=curCloverLength) bottomThickness = curCloverLength;

  G4Tubs *tub = new G4Tubs("tubS",
                            0.,
                            cloverRadius,
                            curCloverLength*0.5,
                            0.*deg,
                            180.*deg);

  G4Box *box = new G4Box("WorldSolid",
                          cloverRadius/sqrt(2.),
                          cloverRadius/sqrt(2.),
                          curCloverLength*0.5);

  G4RotationMatrix* zRot = new G4RotationMatrix; // Rotates X and Z axes only
  zRot->rotateZ(45.*deg); 
  G4UnionSolid* tub_and_box = new G4UnionSolid("tub_and_box",
                                          box,
                                          tub,
                                          zRot,
                                          G4ThreeVector(0,0,0));


  G4Tubs *cavityS = new G4Tubs("cavityS",
                            0.,
                            cavityRadius,
                            curCloverLength*0.5 + 1*mm,
                            0.*deg,
                            360.*deg);

  G4SubtractionSolid* cloverLeafBottomS = new G4SubtractionSolid(
                                        "cloverLeafBottomS",
                                        tub_and_box,
                                        cavityS,
                                        0,
                                        G4ThreeVector(-cloverRadius/sqrt(2.),-cloverRadius/sqrt(2.),bottomThickness+2.*mm)
                                        );
  G4LogicalVolume *cloverLeafBottomLV = new G4LogicalVolume(cloverLeafBottomS, fGe, "cloverLeafBottomLV");

  G4SubtractionSolid* cloverLeafOtherS = new G4SubtractionSolid(
                                        "cloverLeafOtherS",
                                        tub_and_box,
                                        cavityS,
                                        0,
                                        G4ThreeVector(-cloverRadius/sqrt(2.),-cloverRadius/sqrt(2.),0)
                                        );
  G4LogicalVolume *cloverLeafOtherLV = new G4LogicalVolume(cloverLeafOtherS, fGe, "cloverLeafOtherLV");

  G4double zpos0 = -(cloverLength*0.5 - curCloverLength*0.5);
  G4int copyno = 0;
  //first the bottom segment
  for(G4int i=0;i<4;i++) {
    G4ThreeVector pos(cloverRadius/sqrt(2.),cloverRadius/sqrt(2.),zpos0);
    G4RotationMatrix* Rot = new G4RotationMatrix;
    Rot->rotateZ(i*90*deg);
    
    pos = (*Rot) * pos;

    G4String name = "cloverLeafPV" + std::to_string(copyno);
    new G4PVPlacement(G4Transform3D(*Rot,pos),
                      cloverLeafBottomLV,            //its logical volume
                      name,               //its name
                      WorldLV,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno++,                     //copy number
                      checkOverlaps);        //overlaps checking
  }

  // now the other ones
  for(G4int segment=1;segment<nVerticalSegments;segment++) {
    G4double zpos = segment*curCloverLength + zpos0;
    for(G4int i=0;i<4;i++) {
      G4ThreeVector pos(cloverRadius/sqrt(2.),cloverRadius/sqrt(2.),zpos);
      G4RotationMatrix* Rot = new G4RotationMatrix;
      Rot->rotateZ(i*90*deg);
      
      pos = (*Rot) * pos;

      G4String name = "cloverLeafPV" + std::to_string(copyno);
      new G4PVPlacement(G4Transform3D(*Rot,pos),
                        cloverLeafOtherLV,            //its logical volume
                        name,               //its name
                        WorldLV,                     //its mother  volume
                        false,                 //no boolean operation
                        copyno++,                     //copy number
                        checkOverlaps);        //overlaps checking
    }
  }
  
  //====================================================

  return WorldPV; //must return G4VPhysicalVolume pointer to the world
}


G4VPhysicalVolume* DetectorConstruction::SegmentedClover2()
{
  DetectorType = 5;
  //============= MATERIAL DEFINITION =================
  G4NistManager* nist = G4NistManager::Instance();  // Get nist material manager
  //G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic"); //Build vacuum material using the nist manager
  //G4Material* BGO = nist->FindOrBuildMaterial("G4_BGO"); //Build vacuum material using the nist manager
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Build vacuum material using the nist manager
  G4Material* fAlu = nist->FindOrBuildMaterial("G4_Al"); //Build vacuum material using the nist manager

  G4cout << "****************" << G4endl;
  G4cout << "nVerticalSegments = " << nVerticalSegments << G4endl;
  G4cout << "****************" << G4endl;

  //============= GEOMETRY DESCRIPTION =================

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  
  //========== World ==================================
  G4double world_sizeXY = 50.*cm;
  G4double world_sizeZ  = 50.*cm;

  G4Box* solidWorld =
    new G4Box("WorldSolid",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size


  WorldLV =
    new G4LogicalVolume(solidWorld,          //its solid
                        air,           //its material
                        "WorldLV");            //its name

  G4VPhysicalVolume* WorldPV =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),       //at (0,0,0)
                      WorldLV,            //its logical volume
                      "WorldPV",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // Force the detector to be drawn with wireframe
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetForceWireframe(true);
  WorldLV->SetVisAttributes(WorldVisAtt);

  //============== Aluminum casing around the detector ================
  G4double bottomThickness = cloverLength - cavityDepth;
  G4double curCloverLength = cloverLength/nVerticalSegments;
  if(bottomThickness>=curCloverLength) bottomThickness = curCloverLength;
  G4double minWallThickness = 0.1*mm;
  G4double innerRad = cavityRadius-minWallThickness;
  G4double outerRad = cloverRadius*(1.+tan(30*deg))+minWallThickness;
  G4double zPlane1[] = {-(cloverLength*0.5+minWallThickness),
                        -(cloverLength*0.5+minWallThickness),
                        (bottomThickness-cloverLength*0.5)+minWallThickness,
                        (bottomThickness-cloverLength*0.5)+minWallThickness,
                        +cloverLength*0.5+minWallThickness,
                        +cloverLength*0.5+minWallThickness};
  G4double rInner1[] = {0,0,0,innerRad,innerRad,innerRad};
  G4double rOuter1[] = {outerRad,outerRad,outerRad,outerRad,outerRad,outerRad};

  G4Polyhedra *casingS = new G4Polyhedra("casingS",
                                        0,
                                        360.*deg,
                                        6,
                                        6,
                                        zPlane1,
                                        rInner1,
                                        rOuter1);

  G4LogicalVolume *casingLV = new G4LogicalVolume(casingS, fAlu, "casingLV");

  G4RotationMatrix rotDetector;
  rotDetector.rotateX(-90*deg);

  /*G4VPhysicalVolume* casingPV =*/
    new G4PVPlacement(G4Transform3D(rotDetector,G4ThreeVector(0,0,0)),
                      casingLV,            //its logical volume
                      "casingPV",               //its name
                      WorldLV,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // ========== Detector crystal =======

  G4double zPlane[] = {-curCloverLength*0.5,-curCloverLength*0.5,bottomThickness-curCloverLength*0.5,bottomThickness-curCloverLength*0.5,+curCloverLength*0.5,+curCloverLength*0.5};
  G4double rInner[] = {0,0,0,cavityRadius,cavityRadius,cavityRadius};
  G4double rOuter[] = {cloverRadius,cloverRadius,cloverRadius,cloverRadius,cloverRadius,cloverRadius};
  G4Polyhedra *hexagonS = new G4Polyhedra("hexagonS",
                                        0,
                                        60.*deg,
                                        1,
                                        6,
                                        zPlane,
                                        rInner,
                                        rOuter);

  G4Tubs *tub = new G4Tubs("tubS",
                            0.,
                            cloverRadius*tan(30*deg),
                            curCloverLength*0.5,
                            0.*deg,
                            180.*deg);
  G4RotationMatrix* Rot = new G4RotationMatrix;
  Rot->rotateZ(60*deg);
  G4ThreeVector pos(cloverRadius*sin(60*deg),cloverRadius*cos(60.*deg),0);

  G4UnionSolid* cloverLeafS = new G4UnionSolid("cloverLeafS",
                                          hexagonS,
                                          tub,
                                          Rot,
                                          pos);

  G4LogicalVolume *cloverLeafBottomLV = new G4LogicalVolume(cloverLeafS, fGe, "cloverLeafBottomLV");

  G4double zPlane2[] = {-curCloverLength*0.5,-curCloverLength*0.5,+curCloverLength*0.5,+curCloverLength*0.5};
  G4double rInner2[] = {cavityRadius,cavityRadius,cavityRadius,cavityRadius};
  G4double rOuter2[] = {cloverRadius,cloverRadius,cloverRadius,cloverRadius};
  G4Polyhedra *hexagonS2 = new G4Polyhedra("hexagonS2",
                                        0,
                                        60.*deg,
                                        1,
                                        4,
                                        zPlane2,
                                        rInner2,
                                        rOuter2);

  G4UnionSolid* cloverLeafS2 = new G4UnionSolid("cloverLeafS2",
                                          hexagonS2,
                                          tub,
                                          Rot,
                                          pos);

  G4LogicalVolume *cloverLeafOtherLV = new G4LogicalVolume(cloverLeafS2, fGe, "cloverLeafOtherLV");
  //====================================================

  G4double zpos0 = -(cloverLength*0.5 - curCloverLength*0.5);
  G4int copyno = 0;
  //first the bottom segment
  for(G4int i=0;i<6;i++) {
    G4ThreeVector curpos(0,0,zpos0);
    G4RotationMatrix* curRot = new G4RotationMatrix;
    curRot->rotateZ(i*60*deg);

    G4String name = "cloverLeafPV" + std::to_string(copyno);
    new G4PVPlacement(G4Transform3D(*curRot,curpos),
                      cloverLeafBottomLV,            //its logical volume
                      name,               //its name
                      casingLV,                     //its mother  volume
                      false,                 //no boolean operation
                      copyno++,                     //copy number
                      checkOverlaps);        //overlaps checking
  }

  G4double spacing_tolerance = 1.e-12*mm;
  // now the other ones
  for(G4int segment=1;segment<nVerticalSegments;segment++) {
    G4double zpos = segment*(curCloverLength+spacing_tolerance) + zpos0;
    for(G4int i=0;i<6;i++) {
      G4ThreeVector curpos(0,0,zpos);
      G4RotationMatrix* curRot = new G4RotationMatrix;
      curRot->rotateZ(i*60*deg);

      G4String name = "cloverLeafPV" + std::to_string(copyno);
      new G4PVPlacement(G4Transform3D(*curRot,curpos),
                        cloverLeafOtherLV,            //its logical volume
                        name,               //its name
                        casingLV,                     //its mother  volume
                        false,                 //no boolean operation
                        copyno++,                     //copy number
                        checkOverlaps);        //overlaps checking
    }
  }

  return WorldPV; //must return G4VPhysicalVolume pointer to the world
}