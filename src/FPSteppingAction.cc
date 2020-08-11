// Aug 5, 2020: hexc and Zachary
// 
// Add stepping action to track energy loss of the primary particle energy loss.
//

#include "FPSteppingAction.hh"
#include "FPEventAction.hh"
#include "FPDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPSteppingAction::FPSteppingAction(
                      FPEventAction* eventAction)
  : G4UserSteppingAction(),
    fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPSteppingAction::~FPSteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPSteppingAction::UserSteppingAction(const G4Step* step)
{
    // Collect energy and track length step by step
    
    // get volume of the current step
    auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    
    // energy deposit
    auto edep = step->GetTotalEnergyDeposit();
          
    if (edep > 0.) {
      fEventAction->AddELoss(edep);
      //G4cout << " Energy deposit (in stepping action): " << G4BestUnit(edep, "Energy") << G4endl;
    }
    //if (edep <= 0.) G4cout << " Energy deposit (in stepping action): " << G4BestUnit(edep, "Energy") << G4endl;
    
    
    /*
    // step length
    G4double stepLength = 0.;
    if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
	stepLength = step->GetStepLength();
    }

    G4ThreeVector position = step->GetPostStepPoint()->GetPosition();

    
    if ( volume == fDetConstruction->GetLoopLV() ) {
	fEventAction->AddEloss_TrackLength(0, edep,stepLength);
    } else if (volume == fDetConstruction->GetPanelLV()) {
	fEventAction->AddEloss_TrackLength(1, edep,stepLength);
    }
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
