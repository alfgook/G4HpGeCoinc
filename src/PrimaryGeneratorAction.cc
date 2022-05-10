// $Id$
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4GenericIon.hh"
#include "G4IonTable.hh"
#include <iostream> 
#include <math.h>  

// Define
PrimaryGeneratorAction::PrimaryGeneratorAction()     
: G4VUserPrimaryGeneratorAction(),
fParticleGun(0)
{ 
  
  G4int nofParticles = 1;
  fParticleGun  = new G4ParticleGun(nofParticles);
  

   G4ThreeVector position(0, 0, -25.*cm);
   G4double energy = 8.*MeV;
   G4ThreeVector direction(0, 0, 1);
  
  // Default particle kinematics
  G4String particleName = "gamma";
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName);

  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(direction);
  fParticleGun->SetParticleEnergy(energy);
  fParticleGun->SetParticlePosition(position);

}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //This "function" is called at the begining of ecah event
  // Monochromatic pencil like neutron beam

  fParticleGun->GeneratePrimaryVertex(event);

  
}

  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

