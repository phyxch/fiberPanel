/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

/// June 1, 2020: Add optical processes (done!)

/// \file fiberPanelMain.cc

#include "G4Types.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"

#include "Randomize.hh"

#include "FPDetectorConstruction.hh"
#include "FTFP_BERT.hh"
#include "G4PhysListFactory.hh"
#include "G4OpticalPhysics.hh"
//#include "FPPhysicsList.hh"

#include "FPActionInitialization.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  //
  runManager->SetUserInitialization(new FPDetectorConstruction);

  //  The following is from the Example 3a. We don't use it.
  //  runManager->SetUserInitialization(new FPPhysicsList);

  // Need to add code for activating optical physics processes
  // We simply took the code from the loopPanel processes configuration 
  G4PhysListFactory factory;
  G4VModularPhysicsList* phys = NULL;
  G4String physName = "";
  char* path = getenv("PHYSLIST");
  if (path) {
      physName = G4String(path);
  } else {
      physName = "FTFP_BERT"; // default
  }
  // reference PhysicsList via its name
  if (factory.IsReferencePhysList(physName)) {
      phys = factory.GetReferencePhysList(physName);
  }
  //
  // Now add and configure optical physics
  //
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  phys->RegisterPhysics(opticalPhysics);
  opticalPhysics->Configure(kCerenkov, true);
  opticalPhysics->SetCerenkovStackPhotons(false);
  opticalPhysics->Configure(kScintillation, true); 
  opticalPhysics->Configure(kBoundary, true);      
  phys->DumpList();
  
  //auto physicsList = new FTFP_BERT;
  runManager->SetUserInitialization(phys);

  // Set user action initialization
  //
  runManager->SetUserInitialization(new FPActionInitialization());

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
