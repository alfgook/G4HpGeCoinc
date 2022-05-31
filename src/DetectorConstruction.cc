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

//inlcudes for the sensitive detector
#include "HPGeSD.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{ }

//Destructor
DetectorConstruction::~DetectorConstruction()
{ }



//Define the detector
G4VPhysicalVolume* DetectorConstruction::Construct()
{  

  //============= MATERIAL DEFINITION =================
  G4NistManager* nist = G4NistManager::Instance();  // Get nist material manager
  G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic"); //Build vacuum material using the nist manager
  //G4Material* BGO = nist->FindOrBuildMaterial("G4_BGO"); //Build vacuum material using the nist manager
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Build vacuum material using the nist manager
  G4Material* fAlu = nist->FindOrBuildMaterial("G4_Al"); //Build vacuum material using the nist manager

  // Ge-crystal material
  G4Material *fGe = new G4Material("HPGe",32.,72.640*g/mole,5.323*g/cm3,kStateSolid);

  //============= GEOMETRY DESCRIPTION =================

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  
  //========== World ==================================
  G4double world_sizeXY = 50.*cm;
  G4double world_sizeZ  = 50.*cm;

  G4Box* solidWorld =
    new G4Box("WorldSolid",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size


  G4LogicalVolume* WorldLV =
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  HPGeSD* HPGeDetector = new HPGeSD("HPGeSD","HPGeHC");
  G4LogicalVolumeStore::GetInstance()->GetVolume("SmallSegmentLV")->SetSensitiveDetector(HPGeDetector);
  G4LogicalVolumeStore::GetInstance()->GetVolume("BigSegmentLV")->SetSensitiveDetector(HPGeDetector);
  G4SDManager::GetSDMpointer()->AddNewDetector(HPGeDetector);
}