
#include "G4PhysicsListHelper.hh"

#include "PhysicsList.hh" 
#include "G4hMultipleScattering.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLivermorePhysics.hh"

#include "G4ParticleTypes.hh"
#include "G4Radioactivation.hh"
#include "G4NuclearLevelData.hh"
#include "G4NuclideTable.hh"
#include "G4LossTableManager.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4NuclearLevelData.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4DecayPhysics.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4HadronPhysicsQGSP_BERT_HP.hh"

#include "G4RegionStore.hh"

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

  defaultCutValue = 1.0* mm;

  SetVerboseLevel(0);

  fParticleList = new G4DecayPhysics();
  // EM physics
  //emPhysicsList = new G4EmStandardPhysics_option4(1);
  //emPhysicsList = new G4EmLivermorePhysics(1);
  emPhysicsList = new G4EmPenelopePhysics(1);

  hadronPhysicsList = new G4HadronPhysicsQGSP_BERT_HP(1);
  //RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP(1));
}


PhysicsList::~PhysicsList()
{
	delete emPhysicsList;
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::ConstructParticle()
{
  /*
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();
  */
  fParticleList->ConstructParticle();
  emPhysicsList->ConstructParticle();
  hadronPhysicsList->ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
  // transportation
  AddTransportation();

  //
	emPhysicsList->ConstructProcess();
  fParticleList->ConstructProcess();
  hadronPhysicsList->ConstructProcess();

  // decays
  G4Radioactivation* radioactiveDecay = new G4Radioactivation();

  G4bool ARMflag = true;
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
  G4VUserPhysicsList::SetCutValue(0.1*mm, "e-");
  G4VUserPhysicsList::SetCutValue(0.1*mm, "e+");
  G4VUserPhysicsList::SetCutValue(0.01*mm, "gamma");
/*
  G4String regName = "DetectorRegion";
  G4Region *region = G4RegionStore::GetInstance()->GetRegion(regName);
  G4ProductionCuts *cuts = new G4ProductionCuts;
  cuts->SetProductionCut(0.01*mm); // same cuts for gamma, proton, e- and e+
  region->SetProductionCuts(cuts);
*/
  DumpCutValuesTable();
}
