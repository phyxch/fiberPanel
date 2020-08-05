/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

#include "FPActionInitialization.hh"
#include "FPRunAction.hh"
#include "FPEventAction.hh"
#include "FPSteppingAction.hh"
#include "FPPrimaryGeneratorAction.hh"
#include "FPStackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPActionInitialization::FPActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPActionInitialization::~FPActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPActionInitialization::BuildForMaster() const
{
  SetUserAction(new FPRunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPActionInitialization::Build() const
{
  FPRunAction* runAction = new FPRunAction();
  SetUserAction(runAction);

  FPEventAction* evtAction = new FPEventAction(runAction);
  SetUserAction(evtAction);
  
  SetUserAction(new FPPrimaryGeneratorAction);
  SetUserAction(new FPStackingAction);

  SetUserAction(new FPSteppingAction(evtAction));  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
