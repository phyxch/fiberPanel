/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
/// Updated: June 3, 2020: hexc, Zachary and Nadia
///                 Clean up the code and add event output info.
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
  
  /*
  // Get hits collections IDs
  if (fCollID_cryst < 0) {
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();  
  fCollID_cryst   = SDMan->GetCollectionID("crystal/edep");
  fCollID_patient = SDMan->GetCollectionID("patient/dose");    
  }
  
  //Energy in crystals : identify 'good events'
  //
  const G4double eThreshold = 500*keV;
  G4int nbOfFired = 0;
  
  G4THitsMap<G4double>* evtMap = 
  (G4THitsMap<G4double>*)(HCE->GetHC(fCollID_cryst));
  
  std::map<G4int,G4double*>::iterator itr;
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
  ///G4int copyNb  = (itr->first);
  G4double edep = *(itr->second);
  if (edep > eThreshold) nbOfFired++;
  ///G4cout << "\n  cryst" << copyNb << ": " << edep/keV << " keV ";
  }  
  if (nbOfFired == 2) fRunAction->CountEvent();
  
  //Dose deposit in patient
  //
  G4double dose = 0.;
  
  evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(fCollID_patient));
  
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
  ///G4int copyNb  = (itr->first);
  dose = *(itr->second);
  }
  if (dose > 0.) fRunAction->SumDose(dose);
  */
  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
