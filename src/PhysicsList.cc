
#include "G4PhysicsListHelper.hh"

#include "PhysicsList.hh" 
#include "G4hMultipleScattering.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

// Physic lists (contained inside the Geant4 distribution)
#include "G4EmStandardPhysics_option4.hh" /// use for "standard EM". Designed for any applications required higher accuracy of electrons, hadrons and ion tracking without magnetic field. 
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

// For units of measurements
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList(): G4VModularPhysicsList()
					
{
  defaultCutValue = 0.1* mm;

  SetVerboseLevel(0);

  //default physics
  fParticleList = new G4DecayPhysics();

  //default physics
  fRaddecayList = new G4RadioactiveDecayPhysics();

  // EM physics
      emPhysicsList = new G4EmStandardPhysics_option4();
   //   emPhysicsList = new G4EmLivermorePhysics(0);
   //   emPhysicsList = new G4EmPenelopePhysics(0);
}


PhysicsList::~PhysicsList()
{
	delete emPhysicsList;
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::ConstructParticle()
{
  fParticleList->ConstructParticle();
	emPhysicsList->ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
	emPhysicsList->ConstructProcess();

  // decays
  fParticleList->ConstructProcess();
  fRaddecayList->ConstructProcess();
	
  // transportation
  AddTransportation();
}

void PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCutsWithDefault();
  DumpCutValuesTable();
}
