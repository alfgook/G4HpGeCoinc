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
#include "G4Radioactivation.hh"
#include "G4DecayProducts.hh"
#include "G4DecayTable.hh"
#include "G4GenericMessenger.hh"
#include "G4Gamma.hh"

// Define
PrimaryGeneratorAction::PrimaryGeneratorAction()     
: G4VUserPrimaryGeneratorAction(),
fParticleGun(0)
{ 

  fVerbose = 0;

  fParticleGun  = new G4ParticleGun(1);
  
  fRadDecay = new G4Radioactivation();

  fMessenger = new G4GenericMessenger(this,"/PrimaryGeneratorAction/", "control the primary particle source");
  fMessenger->DeclareProperty("ionZ", fZ, "Z of primary radioactive ion");
  fMessenger->DeclareProperty("ionA", fA, "A of primary radioactive ion");
  fMessenger->DeclarePropertyWithUnit("ionEx", "keV", fEx, "excitation energy (keV) of primary radioactive ion");
  fMessenger->DeclareProperty("verbose", fVerbose, "verbose level");

  // default ion is Cs-134
  fZ = 55;
  fA = 134;
  fEx = 0.;
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  
  fIon = G4IonTable::GetIonTable()->GetIon(fZ,fA,fEx);
  fDecayTable = fRadDecay->GetDecayTable1(fIon);

  if (fDecayTable == 0 || fDecayTable->entries() == 0) {
    // No data in the decay table.
    G4cout << "PrimaryGeneratorAction::GeneratePrimaries : "
           << "decay table not defined for "
           << fIon->GetParticleName() 
           << G4endl;

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("geantino");

    G4double energy = 1.*MeV;

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(energy);
    fParticleGun->GeneratePrimaryVertex(event);
    G4cout << "Error in the activity file shooting geantino!!" << G4endl;
    return;
  }

  if(fVerbose>=1) {
    fDecayTable->DumpInfo();
  }

  G4DecayProducts* products = DoDecay(*fIon);
  G4int numberOfSecondaries = products->entries();
  for (G4int index = 0; index < numberOfSecondaries; index++) {
    G4DynamicParticle *secondary = (*products)[index];
    G4ParticleDefinition* particleDef = (G4ParticleDefinition*) secondary->GetParticleDefinition();
    fParticleGun->SetParticleDefinition(particleDef); 
    fParticleGun->SetParticleEnergy(secondary->GetKineticEnergy());
    fParticleGun->SetParticleMomentumDirection(secondary->GetMomentumDirection());
    fParticleGun->GeneratePrimaryVertex(event);
  }

  if(!numberOfSecondaries) {
    G4cout << "PrimaryGeneratorAction::GeneratePrimaries : "
           << "decay table not defined for "
           << fIon->GetParticleName() 
           << G4endl;

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("geantino");

    G4double energy = 1.*MeV;

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(energy);
    fParticleGun->GeneratePrimaryVertex(event);
    G4cout << "Error in the activity file shooting geantino!!" << G4endl;
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4DecayProducts*
PrimaryGeneratorAction::DoDecay(const G4ParticleDefinition& theParticleDef)
{
  G4DecayProducts* products = nullptr;
  // Choose a decay channel.

  // G4DecayTable::SelectADecayChannel checks to see if sum of daughter masses
  // exceeds parent mass. Pass it the parent mass + maximum Q value to account
  // for difference in mass defect.
  G4double parentPlusQ = theParticleDef.GetPDGMass() + 30.*MeV;
  G4VDecayChannel* theDecayChannel = fDecayTable->SelectADecayChannel(parentPlusQ);

  if (!theDecayChannel) {
    // Decay channel not found.
    G4ExceptionDescription ed;
    ed << " Cannot determine decay channel for " << theParticleDef.GetParticleName() << G4endl;
    G4Exception("G4RadioactiveDecay::DoDecay", "HAD_RDM_013",
                FatalException, ed);
  } else {
    // A decay channel has been identified, so execute the DecayIt.
    //G4cout << "G4RadioactiveDecay::DoIt : selected decay channel addr: "
    //       << theDecayChannel << G4endl;

    products = theDecayChannel->DecayIt(theParticleDef.GetPDGMass() );
  }

  return products;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

