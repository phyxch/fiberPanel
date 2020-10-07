/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Updated: July 24, 2020 hexc, Nadia and Zachary
///         Cleaned up the code and added total photon counts at the end of Run
///
///Updated:  Sep 23, 2020 hexc & Zachary
///         Added analyzing histograms for photons collected by SiPM
///         

#include "FPRunAction.hh"
#include "FPPrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RootAnalysisManager.hh"

//#include "g4root.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPRunAction::FPRunAction()
 : G4UserRunAction(),
   fPhotons(0)
{  
  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fPhotons);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPRunAction::~FPRunAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPRunAction::BeginOfRunAction(const G4Run* run)
{ 
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  
  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();

  G4RootAnalysisManager* analysisManager = G4RootAnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  // Open an output file  
  analysisManager->OpenFile("fiberPanel.root"); 
  // Create histograms  
  analysisManager->CreateH1("nPhotons", "Number of Photons", 100, 0, 100);  
  analysisManager->CreateH1("eLoss", "ELoss", 300, 0, 3);  
  
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPRunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
  
  // Merge accumulables 
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const FPPrimaryGeneratorAction* generatorAction
    = static_cast<const FPPrimaryGeneratorAction*>(
        G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String partName;
  if (generatorAction) 
  {
    G4ParticleDefinition* particle 
      = generatorAction->GetParticleGun()->GetParticleDefinition();
    partName = particle->GetParticleName();
  }  
          
  // Print results
  //
  if (IsMaster())
  {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------"
     << G4endl
     << "  The run was " << nofEvents << " events ";
  }
  else
  {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------"
     << G4endl
     << "  The run was " << nofEvents << " "<< partName;
  }
  
  G4cout
     << "; Number of photons " << fPhotons.GetValue()  << G4endl
     << "------------------------------------------------------------" << G4endl 
     << G4endl;

  // Get analysis manager
  G4RootAnalysisManager* analysisManager = G4RootAnalysisManager::Instance();
  // Write and close the output file
  analysisManager->Write();  
  analysisManager->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
