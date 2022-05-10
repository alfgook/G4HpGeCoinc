#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"
#include "G4EmConfigurator.hh"

class G4VPhysicsConstructor;

class PhysicsList: public G4VModularPhysicsList {
public:
  
  PhysicsList();

  virtual ~PhysicsList();

  void ConstructParticle();

  void ConstructProcess();
  void SetCuts();
  

private:
  G4EmConfigurator em_config;

  G4VPhysicsConstructor*               emPhysicsList;
  G4VPhysicsConstructor*               fParticleList;
  G4VPhysicsConstructor*               fRaddecayList;

};
#endif







