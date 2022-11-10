/// \file EnDep.cc
/// \brief Main program of the simulation


// USER DEFINED:
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"

// COMPULSORY
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif


#include "G4UImanager.hh"

// FOR VISUALIZATION
#include "G4VisExecutive.hh"
// USER INTERFACE MANAGER
#include "G4UIExecutive.hh"

// ADDITIONALS
#include <stdio.h>
#include "globals.hh"
#include "Randomize.hh"
#include "G4PhysListFactory.hh"


int main(int argc,char** argv)
{

  /////////////////////////////////////////////////////////////////////  
  // Detect interactive mode (if no arguments) and define UI session
  /////////////////////////////////////////////////////////////////////

  G4String macro;
  #ifdef G4MULTITHREADED
    G4int nThreads = 1;
  #endif

  if( argc>1 ) {
    for ( G4int i=1; i<argc; i=i+2 ) {
      if( G4String(argv[i]) == "-m" ) {
        macro = argv[i+1];
      }
      if( G4String(argv[i]) == "-t" ) nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
  }

  G4UIExecutive* ui = 0;
  if ( macro.size()==0 ) {
    ui = new G4UIExecutive(argc, argv);
  }
  
  ////////////////////////////////
  // Choose the Random engine
  ////////////////////////////////
 
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  //G4long t=time(NULL);
  //G4long myseed =  ((G4long) ((t%333564)*6745)+5);
  //myseed=(long) fabs(myseed);
  //CLHEP::HepRandom::setTheSeed(myseed);
  
  //////////////////////////////////////////
  // Construct the run manager
  //////////////////////////////////////////
  
  #ifdef G4MULTITHREADED
    G4MTRunManager * runManager = new G4MTRunManager;
    
    if(nThreads==0) nThreads = G4Threading::G4GetNumberOfCores();
    runManager->SetNumberOfThreads(nThreads);

  #else
    G4RunManager * runManager = new G4RunManager;
  #endif

  ////////////////////////////////////////////
  // set mandatory initialization classes
  ////////////////////////////////////////////
  
  runManager->SetUserInitialization(new DetectorConstruction());

  // initialize modular physics list defined in ./src/PhysicsList.cc
  runManager->SetUserInitialization(new PhysicsList);

/*
  // alternatively reference pre-compiled PhysicsList via its name
  G4PhysListFactory factory;
  G4VModularPhysicsList* physics = 0;
  physics = factory.GetReferencePhysList("QGSP_BERT_HP");
  runManager->SetUserInitialization(physics);
*/

  // initialization of user action classes
  runManager->SetUserInitialization(new ActionInitialization());

  ///////////////////////////
  // Initialize G4 kernel
  ///////////////////////////
  
  runManager->Initialize();
  
  
  //////////////////////////////////////////////////////////////
  // Initialize visualization
  // This part has to be after the run manager initialization
  //////////////////////////////////////////////////////////////
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();


  
  
  ////////////////////////////////////////////////////////////
  // Get the pointer to the User Interface manager
  // This part has to be after the run manager initialization
  ////////////////////////////////////////////////////////////
  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if (!ui) {// batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else {// interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute detectorSetup.mac");
    UImanager->ApplyCommand("/control/execute init_vis.mac"); 
    if (ui->IsGUI()) {
       UImanager->ApplyCommand("/control/execute icons.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  
  
  
  ///////////////////////////
  // Free memory
  ///////////////////////////

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

  delete visManager;
  delete runManager;
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
