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

#include "FPEventAction.hh"
#include "FPRunAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPEventAction::FPEventAction(FPRunAction* runAction)
 : G4UserEventAction(), 
   fRunAction(runAction)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPEventAction::~FPEventAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPEventAction::BeginOfEventAction(const G4Event* /*evt*/)
{ }

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

  G4THitsMap<G4int>* evtMap = 
  (G4THitsMap<G4int>*)(HCE->GetHC(HCID));
  
  std::map<G4int,G4int*>::iterator itr;
  
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    //  G4int photonCount  = (itr->first);
    G4int photonCount  = *(itr->photonCounts);
  }  

  // Ask Run action to update photon counts
  if (photonCount > 0)
  fRunAction->CountPhoton();

}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
