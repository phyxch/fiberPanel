/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Updated: June 3, 2020: hexc, Zachary and Nadia
///                 Clean up the code and add event output info.
///
/// Updated: July 24, 2020: hexc, Zachary and Nadia
///                 Clean up the code and pass photon count to RunAction.
///

#ifndef FPEventAction_h
#define FPEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class FPRunAction;

class FPEventAction : public G4UserEventAction
{
  public:
    FPEventAction(FPRunAction* runAction);
    virtual ~FPEventAction();

    virtual void  BeginOfEventAction(const G4Event*);
    virtual void    EndOfEventAction(const G4Event*);
    
  private:
    FPRunAction*  fRunAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
