//
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4Event;
class G4ParticleGun;
class G4GenericMessenger;
class G4Radioactivation;
class G4DecayProducts;
class G4ParticleDefinition;
class G4DecayTable;
class G4GenericMessenger;
class G4SPSPosDistribution;
class G4SPSRandomGenerator;

/// The first primary generator action class. It is of the type G4VUserPrimaryGeneratorAction

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
    virtual ~PrimaryGeneratorAction();
     // method from the base class
    virtual void GeneratePrimaries(G4Event*); 

  private:
   // G4GenericMessenger*  fMessenger;
    G4ParticleGun*  fParticleGun;  
    G4Radioactivation* fRadDecay;
    G4GenericMessenger* fMessenger;
    G4ParticleDefinition *fIon;
    G4DecayTable* fDecayTable;
    G4SPSPosDistribution* fPosDist;
    G4SPSRandomGenerator* fPosGenerator;

    // parameters controlled trhough the generic messenger
    G4int fZ;
    G4int fA;
    G4double fEx;
    G4int fVerbose;

    G4DecayProducts* DoDecay(const G4ParticleDefinition& theParticleDef);
};

#endif


