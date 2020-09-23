/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
/// Updated: June 3, 2020: hexc, Zachary and Nadia
///                 Clean up the code and add event output info.
///
///
/// Updated: July 24, 2020 hexc, Nadia and Zachary
///         Add photon count to the RunAction
///
/// Updated: August 10, 2020 hexc, Nadia and Zachary
///         Add summary info at the end of each event:  # of steps and the muon eLoss sum.
///Updated:  Sep 23, 2020 hexc & Zachary
///         Added analyzing histograms for photons collected by SiPM
/// 

#include "FPEventAction.hh"
#include "FPRunAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RootAnalysisManager.hh"
//#include "g4root.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPEventAction::FPEventAction(FPRunAction* runAction)
 : G4UserEventAction(),
   fRunAction(runAction)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPEventAction::~FPEventAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPEventAction::BeginOfEventAction(const G4Event* /*evt*/)
{
  // Initialize the total energy loss and the total number of steps
  totalEloss = 0.0;
  totalSteps = 0;
}

void FPEventAction::AddELoss(G4double eLoss)
{
  // Initialize the total energy loss and the total number of steps
  totalEloss += eLoss;
  totalSteps += 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPEventAction::EndOfEventAction(const G4Event* evt )
{
  
  //Hits collections
  //  
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(!HCE) return;

  // Get hits collections IDs
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();  
  G4int HCID   = SDMan->GetCollectionID("SiPMHitCollection");

  auto hc = HCE->GetHC(HCID);
  if (hc->GetSize() > 0) {
    G4cout << "The size of the Hit Collection of This Event: " << hc->GetSize() << G4endl;
    fRunAction->CountPhoton();
  }

  G4cout << "Number of tracking steps: " << totalSteps << "     Total ELoss: " << G4BestUnit(totalEloss, "Energy")<< G4endl;

  G4RootAnalysisManager* analysisManager = G4RootAnalysisManager::Instance();
  analysisManager->FillH1(0, hc->GetSize());
  analysisManager->FillH1(1, totalEloss);

  /*
  G4THitsMap<G4int>* evtMap =  (G4THitsMap<G4int>*)(HCE->GetHC(HCID));
  
  std::map<G4int,G4int*>::iterator itr;
  
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    //  G4int photonCount  = (itr->first);
    G4int photonCount  = *(itr->photonCounts);
  }  

  // Ask Run action to update photon counts
  if (photonCount > 0) fRunAction->CountPhoton();
  */
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
