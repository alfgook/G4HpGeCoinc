//
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4Event;
class G4ParticleGun;
class G4GenericMessenger;

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
   // G4bool          fRandom;  
};

#endif


