
#include "G4PhysicsListHelper.hh"

#include "PhysicsList.hh" 
#include "G4hMultipleScattering.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4ParticleTypes.hh"
#include "G4IonConstructor.hh"
#include "G4Radioactivation.hh"
#include "G4NuclearLevelData.hh"
#include "G4NuclideTable.hh"
#include "G4LossTableManager.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4NuclearLevelData.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4DecayPhysics.hh"

// For units of measurements
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList(): G4VModularPhysicsList()
					
{
  // mandatory for G4NuclideTable
  //
  G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(0.1*picosecond);
  G4NuclideTable::GetInstance()->SetLevelTolerance(1.0*eV);

  //read new PhotonEvaporation data set 
  //
  G4DeexPrecoParameters* deex = 
    G4NuclearLevelData::GetInstance()->GetParameters();
  deex->SetCorrelatedGamma(true);
  deex->SetStoreAllLevels(true);
  deex->SetIsomerProduction(true);  
  deex->SetMaxLifeTime(G4NuclideTable::GetInstance()->GetThresholdOfHalfLife()
                /std::log(2.));

  defaultCutValue = 0.1* mm;

  SetVerboseLevel(0);

  fParticleList = new G4DecayPhysics();
  // EM physics
  emPhysicsList = new G4EmStandardPhysics_option4();
   //   emPhysicsList = new G4EmLivermorePhysics(0);
   //   emPhysicsList = new G4EmPenelopePhysics(0);
}


PhysicsList::~PhysicsList()
{
	delete emPhysicsList;
  delete fParticleList;
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::ConstructParticle()
{
  
  emPhysicsList->ConstructParticle();
  fParticleList->ConstructParticle();
  
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  
  // gamma
  G4Gamma::GammaDefinition();

  // leptons
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();

  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  
  // baryons
  G4Proton::ProtonDefinition();
  G4Neutron::NeutronDefinition();  

  // ions
  G4IonConstructor iConstructor;
  iConstructor.ConstructParticle(); 
}

void PhysicsList::ConstructProcess()
{
  // transportation
  AddTransportation();

  // em physics
	emPhysicsList->ConstructProcess();
  fParticleList->ConstructProcess();

  // decays
  G4Radioactivation* radioactiveDecay = new G4Radioactivation();

  G4bool ARMflag = false;
  radioactiveDecay->SetARM(ARMflag);        //Atomic Rearangement

  // need to initialize atomic deexcitation
  //
  G4LossTableManager* man = G4LossTableManager::Instance();
  G4VAtomDeexcitation* deex = man->AtomDeexcitation();
  if (!deex) {
     ///G4EmParameters::Instance()->SetFluo(true);
     G4EmParameters::Instance()->SetAugerCascade(ARMflag);
     G4EmParameters::Instance()->SetDeexcitationIgnoreCut(ARMflag);    
     deex = new G4UAtomicDeexcitation();
     deex->InitialiseAtomicDeexcitation();
     man->SetAtomDeexcitation(deex);
  }

  // register radioactiveDecay
  //
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  ph->RegisterProcess(radioactiveDecay, G4GenericIon::GenericIon());

  //printout
  //
  G4cout << "\n  Set atomic relaxation mode " << ARMflag << G4endl;
}

void PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCutsWithDefault();
  DumpCutValuesTable();
}
