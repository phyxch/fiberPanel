// Aug 5, 2020: Hexc and Zachary
//
// Add stepping action to track energy loss of the primary particle energy loss.

#ifndef FPSteppingAction_h
#define FPSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class FPDetectorConstruction;
class FPEventAction;

/// Stepping action class.
///
/// In UserSteppingAction() there are collected the energy deposit and track 
/// lengths of charged particles in Absober and Gap layers and
/// updated in B4aEventAction.

class FPSteppingAction : public G4UserSteppingAction
{
public:
  FPSteppingAction(FPEventAction* eventAction);
  virtual ~FPSteppingAction();

  virtual void UserSteppingAction(const G4Step* step);
    
private:
  FPEventAction*  fEventAction;  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
