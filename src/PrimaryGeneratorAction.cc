// $Id$
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
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
#include "G4SPSPosDistribution.hh"
#include "G4SPSRandomGenerator.hh"
#include "G4PhysicalVolumeStore.hh"

// Define
PrimaryGeneratorAction::PrimaryGeneratorAction()     
: G4VUserPrimaryGeneratorAction(),
fParticleGun(0),
fGPS(0)
{ 

  fVerbose = 0;

  fParticleGun  = new G4ParticleGun(1);
  fGPS  = new G4GeneralParticleSource();
  fUseGPS = false;
  
  fRadDecay = new G4Radioactivation();

  fMessenger = new G4GenericMessenger(this,"/PrimaryGeneratorAction/", "control the primary particle source");
  fMessenger->DeclareProperty("ionZ", fZ, "Z of primary radioactive ion");
  fMessenger->DeclareProperty("ionA", fA, "A of primary radioactive ion");
  fMessenger->DeclarePropertyWithUnit("ionEx", "keV", fEx, "excitation energy (keV) of primary radioactive ion");
  fMessenger->DeclareProperty("verbose", fVerbose, "verbose level");
  fMessenger->DeclareProperty("followdecaychain", fFollowDecayChain, "wether (1) or not (0) the daugther of the primary ion should be decayed");
  fMessenger->DeclareProperty("useGPS", fUseGPS, "set to true to use the general particle source, or false (default) to turn it off");

  fMessenger->DeclareMethod("setVolumeSource", &PrimaryGeneratorAction::SetVolumeSource)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("set the source to the given physical volume name. WARNING! This only works for volumes placed at (0,0,0) or if the volume is placed directly in the world (not nested).")
    .SetParameterName("volume_name", false);

  fMessenger->DeclareMethod("setSourceGeometryType", &PrimaryGeneratorAction::SetSourceGeometryType)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("todo");

//    .SetParameterName("volume_name", false);

  // default ion is La-140
  fZ = 57;
  fA = 140;
  fEx = 0.;

  // position distribution
  fPosDist = new G4SPSPosDistribution();
  fPosGenerator = new G4SPSRandomGenerator();
  fPosDist->SetBiasRndm(fPosGenerator);
  fPosDist->SetPosDisType("Volume");
  fPosDist->SetPosDisShape("Cylinder");
  fPosDist->SetRadius(3.0*mm);
  fPosDist->SetHalfZ(1.5*mm);

  fPosMessenger = new G4GenericMessenger(fPosDist,"/PrimaryGeneratorAction/pos/", "control the primary particle source position distribution");
  fPosMessenger->DeclareMethod("shape", &G4SPSPosDistribution::SetPosDisShape)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("set the shape of the source to Sphere, Ellipsoid, Cylinder or Para (parallelpiped)")
    .SetParameterName("shape", false);
  fPosMessenger->DeclareMethod("center", &G4SPSPosDistribution::SetCentreCoords)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Sets the centre co-ordinates (X,Y,Z) of the source in mm [default (0,0,0)].")
    .SetParameterName("center", false);
  fPosMessenger->DeclareMethod("halfX", &G4SPSPosDistribution::SetHalfX)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Sets the half-length in x in mm [default 0].")
    .SetParameterName("halfX", false);
  fPosMessenger->DeclareMethod("halfY", &G4SPSPosDistribution::SetHalfY)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Sets the half-length in y in mm [default 0].")
    .SetParameterName("halfY", false);
  fPosMessenger->DeclareMethod("halfZ", &G4SPSPosDistribution::SetHalfZ)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Sets the half-length in z in mm [default 0]. Or the half-height of the cylinder.")
    .SetParameterName("halfZ", false);
  fPosMessenger->DeclareMethod("radius", &G4SPSPosDistribution::SetRadius)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Sets the radius in mm of the source [default 0]")
    .SetParameterName("radius", false);
  fPosMessenger->DeclareMethod("paralpha", &G4SPSPosDistribution::SetParAlpha)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Used with a Parallelepiped. The angle [default 0 rad] alpha formed by the y-axis and the plane joining the centre of the faces parallel to the zx plane at y and +y.")
    .SetParameterName("alpha", false);
  fPosMessenger->DeclareMethod("partheta", &G4SPSPosDistribution::SetParTheta)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Used with a Parallelepiped. Polar angle [default 0 rad] theta of the line connecting the centre of the face at z to the centre of the face at +z. The units can only be deg or rad")
    .SetParameterName("theta", false);
  fPosMessenger->DeclareMethod("parphi", &G4SPSPosDistribution::SetParPhi)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Used with a Parallelepiped. The azimuth angle [default 0 rad] phi of the line connecting the centre of the face at z with the centre of the face at +z. The units can only be deg or rad.")
    .SetParameterName("phi", false);
  fPosMessenger->DeclareMethod("confine", &G4SPSPosDistribution::ConfineSourceToVolume)
    .SetStates(G4State_PreInit,  G4State_Idle)
    .SetGuidance("Allows the user to confine the source to the physical volume name [default NULL]. Note that the defined volume source must completely cover the confining volume")
    .SetParameterName("volume_name", false);

  fFollowDecayChain = 1;

  fVolumeSource = nullptr;
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGPS;
}

void PrimaryGeneratorAction::SetVolumeSource(G4String volume_name)
{
  G4PhysicalVolumeStore *volStore = G4PhysicalVolumeStore::GetInstance();
  G4VPhysicalVolume *the_volume = volStore->GetVolume(volume_name);

  G4cout << "PrimaryGeneratorAction::SetVolumeSource(G4String volume_name)" << G4endl;

  if(the_volume) {
    G4cout << "setting the volume " << volume_name << " as the source" << G4endl;

    fVolumeSource = the_volume;

    /* 
      A problem with this is that fVolumeSource->GetTranslation() only gets the local translation (inside it's mother volume).
      It is difficult to get the global location/roation of the volume. One way would be to find the correct volume using the
      G4Navigator at the start of the event-loop, when the geometry is closed. This way I would get a touchable which is a more
      appropriate representation.
    */

  } else {

    fVolumeSource = nullptr;
    G4cout << volume_name << " not found, removing the volume source!" << G4endl;
  
  }
}

void PrimaryGeneratorAction::SetSourceGeometryType(G4String type)
{

}

G4ThreeVector PrimaryGeneratorAction::GenerateVertexPos()
{
  G4ThreeVector decayPos;

  if(fVolumeSource) {
    G4VSolid *the_solid = fVolumeSource->GetMotherLogical()->GetSolid();

    G4ThreeVector lo, hi;
    the_solid->BoundingLimits(lo, hi);

    G4int max_tries = 10000, itry = 0;
    do {
      decayPos.set( lo.x() + G4UniformRand()*(hi.x() - lo.x()),
                    lo.y() + G4UniformRand()*(hi.y() - lo.y()),
                    lo.z() + G4UniformRand()*(hi.z() - lo.z()));
    } while(!the_solid->Inside(decayPos) && ++itry < max_tries);

    if(itry == max_tries) {
      G4cerr << "Was not able to generate vertex position in source" << G4endl;
    }

    decayPos += fVolumeSource->GetTranslation();

  } else {
    decayPos = fPosDist->GenerateOne();
  }

  return decayPos;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  if(fUseGPS) {
    fGPS->GeneratePrimaryVertex(event);
    return;
  }
  
  fIon = G4IonTable::GetIonTable()->GetIon(fZ,fA,fEx);
  fDecayTable = fRadDecay->GetDecayTable1(fIon);

  /*
  G4ThreeVector decayPos = fPosDist->GenerateOne();
  decayPos += G4ThreeVector(0.,0.,+5.*mm);
  */

   G4ThreeVector decayPos = GenerateVertexPos();

  if (fDecayTable == 0 || fDecayTable->entries() == 0) {
    // No data in the decay table.
    if(fVerbose>=1) {
      G4cout << "PrimaryGeneratorAction::GeneratePrimaries : "
           << "decay table not defined for "
           << fIon->GetParticleName() 
           << G4endl;
      G4cout << "Error in the activity file shooting geantino!!" << G4endl;
    }

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("geantino");

    G4double energy = 1.*MeV;

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(energy);
    fParticleGun->GeneratePrimaryVertex(event);
    return;
  }

  if(fVerbose>=1) {
    fDecayTable->DumpInfo();
  }

  /*G4double LifeTime = fIon->GetPDGLifeTime();
  if(LifeTime>0.) time = -LifeTime*log(G4UniformRand());*/

  G4DecayProducts* products = DoDecay(*fIon);
  G4int numberOfSecondaries = 0;
  if(products) {
    numberOfSecondaries = products->entries();
    for (G4int index = 0; index < numberOfSecondaries; index++) {
      G4DynamicParticle *secondary = (*products)[index];
      G4ParticleDefinition* particleDef = (G4ParticleDefinition*) secondary->GetParticleDefinition();
      if(particleDef->GetPDGCharge()>2 && fFollowDecayChain==0) {
        continue; // do not add the daughter nuclide to the source
      }
      fParticleGun->SetParticleDefinition(particleDef); 
      fParticleGun->SetParticleEnergy(secondary->GetKineticEnergy());
      fParticleGun->SetParticleMomentumDirection(secondary->GetMomentumDirection());
      fParticleGun->SetParticlePosition(decayPos);
      fParticleGun->GeneratePrimaryVertex(event);
    }
    delete products;
  }
  

  if(!numberOfSecondaries) {
    if(fVerbose>=1) {
      G4cout << "PrimaryGeneratorAction::GeneratePrimaries : "
           << "decay table not defined for "
           << fIon->GetParticleName() 
           << G4endl;
      G4cout << "Error in the activity file shooting geantino!!" << G4endl;
    }

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("geantino");

    G4double energy = 1.*MeV;

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(energy);
    fParticleGun->GeneratePrimaryVertex(event);
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

