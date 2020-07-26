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

  /*

  // Drift chambers hits
  for (G4int iDet = 0; iDet < kDim; ++iDet) {
    auto hc = GetHC(event, fDriftHCID[iDet]);
    if ( ! hc ) return;

    auto nhit = hc->GetSize();
    analysisManager->FillH1(fDriftHistoID[kH1][iDet], nhit );
    // columns 0, 1
    analysisManager->FillNtupleIColumn(iDet, nhit);
  
    for (unsigned long i = 0; i < nhit; ++i) {
      auto hit = static_cast<B5DriftChamberHit*>(hc->GetHit(i));
      auto localPos = hit->GetLocalPos();
      analysisManager->FillH2(fDriftHistoID[kH2][iDet], localPos.x(), localPos.y());
    }
  }


  auto hce = event->GetHCofThisEvent();
  if (!hce) {
      G4ExceptionDescription msg;
      msg << "No hits collection of this event found." << G4endl; 
      G4Exception("B5EventAction::EndOfEventAction()",
                  "B5Code001", JustWarning, msg);
      return nullptr;
  }

  auto hc = hce->GetHC(collId);
  if ( ! hc) {
    G4ExceptionDescription msg;
    msg << "Hits collection " << collId << " of this event not found." << G4endl; 
    G4Exception("B5EventAction::EndOfEventAction()",
                "B5Code001", JustWarning, msg);
  }
  return hc; 

   */

  
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
