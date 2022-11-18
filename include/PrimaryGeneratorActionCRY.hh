//
/// \file PrimaryGeneratorActionCRY.hh
/// \brief Definition of the PrimaryGeneratorActionCRY class

#ifndef PrimaryGeneratorActionCRY_h
#define PrimaryGeneratorActionCRY_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "CRYParticle.h"

class G4Event;
class G4ParticleGun;
class G4ParticleDefinition;
class PrimaryGeneratorMessenger;
class CRYGenerator;
class G4ParticleTable;
//class CRYParticle;

/// The first primary generator action class. It is of the type G4VUserPrimaryGeneratorAction

class PrimaryGeneratorActionCRY : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorActionCRY();    
    virtual ~PrimaryGeneratorActionCRY();
     // method from the base class
    virtual void GeneratePrimaries(G4Event*); 
    void InputCRY();
    void UpdateCRY(std::string* MessInput);
    G4int CRYFromFile(G4String newValue);
    G4ParticleDefinition* CryParticleDef(CRYParticle::CRYId, int charge);
    //G4ParticleDefinition* CryParticleDef(enum id);

  private:
    G4ParticleGun* particleGun;
    G4ParticleTable* particleTable;
    G4int InputState;
    CRYGenerator* generator;

    PrimaryGeneratorMessenger* gunMessenger;
    std::vector<CRYParticle*> particles; // vector of generated particles

    G4double world_extent;
    G4double world_z_max;

    std::string CRY_setup_string;
    G4double global_simulation_time;
};

#endif


