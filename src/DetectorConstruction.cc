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
#include "G4SDParticleFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4MultiFunctionalDetector.hh"
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

  // LaBr-crystal material
  G4Element *Ce = nist->FindOrBuildElement("Ce");
  G4Element *La = nist->FindOrBuildElement("La");
  G4Element *Br = nist->FindOrBuildElement("Br");
  G4Material* fLaBr = new G4Material("LaBr:Ce",5.08*g/cm3,3,kStateSolid);
  fLaBr->AddElement(Ce,1.25*perCent);
  fLaBr->AddElement(La,23.75*perCent);
  fLaBr->AddElement(Br,75.0*perCent);

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

  // Force the detector to be drawn with wireframe
  G4VisAttributes *WorldVisAtt = new G4VisAttributes();
  WorldVisAtt->SetForceWireframe(true);
  WorldLV->SetVisAttributes(WorldVisAtt);

  // ========== Cylinder representing a detector crystal =======

  //G4double detector_radius = 2.54*cm; // 1 inch
  //G4double detector_length = 2.54*cm; // 1 inch 
  G4double detector_radius = 40.*mm; // 
  G4double detector_length = 90.*mm; //

  // crystal

  G4Tubs* solidDetector = new G4Tubs("DetectorSolid",0.,detector_radius,0.5*detector_length,0.,360.*deg);

  G4LogicalVolume* DetectorLV = 
    new G4LogicalVolume(solidDetector,          //its solid
                        //fLaBr,           //its material
                        fGe,           //its material
                        "DetectorLV");            //its name


  /*G4VPhysicalVolume* DetectorPV =*/
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),  //at (0,0,0)
                      DetectorLV,            //its logical volume
                      "DetectorPV",          //its name
                      WorldLV,               //its mother volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  G4VisAttributes *GeVisAtt = new G4VisAttributes(G4Color::Red());
  DetectorLV->SetVisAttributes(GeVisAtt);
  //====================================================

  SetupDetectors();
  return WorldPV; //must return G4VPhysicalVolume pointer to the world


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetupDetectors()
{
  // we now create a primitive scorer which registers energy deposits
  G4VPrimitiveScorer *scorer = new G4PSEnergyDeposit("eDep",0);

  // Create a filter: we want to register energy deposits from electrons and positrons to model a typical gamma-ray detector
  // We also need to add gammas to our filter: This is because of the way geant4 treats "cuts". If the range of the
  // electron/positron is smaller than the specified range cut ti will not be created and the energy will be counted as
  // having been deposit by the gamma-ray.
  G4String filterName, particleName;
  G4SDParticleFilter* aFilter = new G4SDParticleFilter(filterName="epFilter");
  aFilter->add(particleName="e-");
  aFilter->add(particleName="e+");
  aFilter->add(particleName="gamma");

  // we assign our recently created filter to it
  scorer->SetFilter(aFilter);

  // now we assign the primitive scorrer to a detector, one could add more types of primitive scorers to this same detector
  G4MultiFunctionalDetector* det = new G4MultiFunctionalDetector("theDetector");
  det->RegisterPrimitive(scorer);

  // we then assign the sensitivity to a logicla volume
  G4LogicalVolume *logicVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DetectorLV"); //retrieve it by its name
  logicVolume->SetSensitiveDetector(det);

  // finaly we register the created detector to the detector manager 
  G4SDManager::GetSDMpointer()->AddNewDetector(det);
}