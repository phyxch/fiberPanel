/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

#ifndef FPEventAction_h
#define FPEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class FPRunAction;

/// Event action class
///
/// In EndOfEventAction() there is collected information event per event 
/// from Hits Collections, and accumulated statistic for 
/// B3RunAction::EndOfRunAction().

class FPEventAction : public G4UserEventAction
{
  public:
    FPEventAction(FPRunAction* runAction);
    virtual ~FPEventAction();

    virtual void  BeginOfEventAction(const G4Event*);
    virtual void    EndOfEventAction(const G4Event*);
    
  private:
    FPRunAction*  fRunAction;
    G4int fCollID_cryst;
    G4int fCollID_patient;   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
