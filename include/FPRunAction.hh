/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Updated: July 24, 2020 hexc, Nadia and Zachary
///         Cleaned up the code and added total photon counts at the end of Run
///

#ifndef FPRunAction_h
#define FPRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"

/// Run action class

class FPRunAction : public G4UserRunAction
{
  public:
    FPRunAction();
    virtual ~FPRunAction();
    
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    void CountPhoton()           { fPhotons += 1; };

private:
    G4Accumulable<G4int>    fPhotons;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

