// $Id$
//
/// \file PrimaryGeneratorActionCRY.cc
/// \brief Implementation of the PrimaryGeneratorActionCRY class

#include "PrimaryGeneratorActionCRY.hh"

// geant4 includes
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <iostream> 
#include "G4ParticleTable.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4AnalysisManager.hh"

#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
//#include "G4Kaon.hh"
//#include "G4Pion.hh"

// CRY includes
#include "CRYGenerator.h"
#include "CRYParticle.h"
#include "CRYSetup.h"
#include "CRYUtils.h"

#include "RNGWrapper.hh"
#include "Randomize.hh"
//#define CRY_DATA_DIR "../../data"

// Define
PrimaryGeneratorActionCRY::PrimaryGeneratorActionCRY()     
: G4VUserPrimaryGeneratorAction(),
particleGun(new G4ParticleGun()),
particleTable(G4ParticleTable::GetParticleTable()),
InputState(-1),
generator(nullptr)
{ 
  gunMessenger = new PrimaryGeneratorMessenger(this);
  global_simulation_time = 0;

  if(CRYFromFile("default.cry.setup")) {
    // the setup failed
    G4String* str = new G4String("CRY library was not successfully initialized.");
    G4Exception("PrimaryGeneratorActionCRY", "1",FatalException, *str);
  } else {

    // set the exten in which particles are generated to be the extent of the world
    G4LogicalVolume *worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("WorldLV");
    if(!worldLV) {
      G4String* str = new G4String("Couldn't find the world logical volume. It must have the name 'WorldLV'");
      G4Exception("PrimaryGeneratorActionCRY", "2",FatalException, *str);
    } else if(worldLV->GetSolid()->GetEntityType()!="G4Box") {
      G4String* str = new G4String("The world volume must have be of solid type G4Box");
      G4Exception("PrimaryGeneratorActionCRY", "3",FatalException, *str);
    } else {
      G4VSolid *worldSolid = worldLV->GetSolid();
      G4double xHalfLength = ((G4Box*) worldSolid)->GetXHalfLength();
      G4double yHalfLength = ((G4Box*) worldSolid)->GetYHalfLength();

      world_extent = 2.*std::max(xHalfLength,yHalfLength)/meter;
      world_z_max = ((G4Box*) worldSolid)->GetZHalfLength()/meter;

    }

    G4String cmd = "subboxLength " + std::to_string(world_extent);
    UpdateCRY(&cmd);
    
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PrimaryGeneratorActionCRY::~PrimaryGeneratorActionCRY()
{

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorActionCRY::GeneratePrimaries(G4Event* event)
{
  if (InputState != 0) {
    if(CRYFromFile("default.cry.setup")) {
      G4String str("CRY library was not successfully initialized");
      G4Exception("PrimaryGeneratorActionCRY", "4",RunMustBeAborted, str);

      return;
    }
  }

  G4int nGenerated(0);

  while(!nGenerated) { // loop to make sure at least one particle is generated per event

    particles.clear();
    generator->genEvent(&particles);

    //....debug output
    /*G4cout << "\nEvent=" << event->GetEventID() << " "
           << "CRY generated nparticles=" << particles.size()
           << "CRY time simulated (sec)=" << generator->timeSimulated()
           << G4endl;*/

    // the time (*particle)->t() is the global time, i.e. the total time elapsed in the CRY simulation
    // the rate is around 1/0.01 sec = 1e-01 ms⁻¹ (but depends on the size of the world)
    // I set the time of the track to the local time, ie. the time since the generation of the primary
    // cosmic ray in CRY. Hopefully we will not loose the time correlation due to precision.
    // CRY does use double precision, so 52 bits for the number ~ 1E15, so we should be able to run
    // about 1e6 s before ns precision is lost. What I can do is reinitilize CRY when timeSimulated is
    // too large.
    
    G4double timeSimulated = generator->timeSimulated();

    for(auto particle=particles.begin();particle!=particles.end();particle++) {
      if(G4ParticleDefinition* particleDef = CryParticleDef((*particle)->id(),(*particle)->charge())) {
        particleGun->SetParticleDefinition(particleDef);
        particleGun->SetParticleEnergy((*particle)->ke()*MeV);
        particleGun->SetParticleMomentumDirection(G4ThreeVector((*particle)->u(), (*particle)->v(), (*particle)->w()));
        particleGun->SetParticlePosition(G4ThreeVector((*particle)->x(), (*particle)->y(), (*particle)->z() + world_z_max)*meter);
        particleGun->SetParticleTime(((*particle)->t()-timeSimulated)*second);
        particleGun->GeneratePrimaryVertex(event);
        ++nGenerated;
      }

      delete (*particle);
      
      //....debug output 
      /*G4String pName = CRYUtils::partName((*particle)->id()); 
      G4cout << "  "          << pName << " "
           << "charge="      << (*particle)->charge() << " "
           //<< setprecision(4)
           << "energy (MeV)=" << (*particle)->ke()*MeV << " "
           << "time (sec)=" << (*particle)->t() << " "
           << "pos (m)"
           << G4ThreeVector((*particle)->x(), (*particle)->y(), (*particle)->z())
           << " " << "direction cosines "
           << G4ThreeVector((*particle)->u(), (*particle)->v(), (*particle)->w())
           << "----------------------------------------------------------" << G4endl;*/
    }
  }
  
  if(generator->timeSimulated()>1.e5) { //reinit the CRY generator to not loose time correlations
    global_simulation_time += generator->timeSimulated();
    std::string dummy("");
    UpdateCRY(&dummy);
  }

  G4AnalysisManager::Instance()->FillNtupleDColumn(0,global_simulation_time+generator->timeSimulated());
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorActionCRY::InputCRY()
{
  InputState = 1;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorActionCRY::UpdateCRY(std::string* MessInput)
{
  CRY_setup_string.append(*MessInput);
  CRYSetup *setup = new CRYSetup(CRY_setup_string,CRY_DATA_DIR);

  if(generator) delete generator;
  generator = new CRYGenerator(setup);

  // set random number generator
  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  InputState=0;

  delete setup;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int PrimaryGeneratorActionCRY::CRYFromFile(G4String newValue)
{
  // Read the cry input file
  std::ifstream inputFile;
  inputFile.open(newValue,std::ios::in);
  char buffer[1000];

  if(inputFile.fail()) {
    G4cout << "Failed to open input file " << newValue << G4endl;
    G4cout << "Make sure to define the cry library on the command line" << G4endl;
    InputState=-1;
    return InputState;
  }

  CRY_setup_string.clear();
  while ( !inputFile.getline(buffer,1000).eof()) {
    CRY_setup_string.append(buffer);
    CRY_setup_string.append(" ");
  }
  inputFile.close();

  CRYSetup *setup = new CRYSetup(CRY_setup_string,CRY_DATA_DIR);

  if(generator) delete generator;
  generator = new CRYGenerator(setup);

  // set random number generator
  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  InputState=0;

  delete setup;

  return InputState;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4ParticleDefinition* PrimaryGeneratorActionCRY::CryParticleDef(CRYParticle::CRYId id, int charge)
{
  switch(id)
  {
    case CRYParticle::Neutron: return G4Neutron::Neutron();
    case CRYParticle::Proton: return G4Proton::Proton();
    case CRYParticle::Electron: return G4Electron::Electron();
    case CRYParticle::Gamma: return G4Gamma::Gamma();
    case CRYParticle::Muon:
      switch(charge)
      {
        case -1: return G4MuonMinus::MuonMinus();
        case +1: return G4MuonPlus::MuonPlus();
      }
      return nullptr;

    case CRYParticle::Pion: return nullptr;
    case CRYParticle::Kaon: return nullptr;
  }

  return nullptr;
}