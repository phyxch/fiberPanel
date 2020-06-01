/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

#ifndef FPStackingAction_h
#define FPStackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

/// Stacking action class : manage the newly generated particles
///
/// One wishes do not track secondary neutrino.Therefore one kills it 
/// immediately, before created particles will  put in a stack.

class FPStackingAction : public G4UserStackingAction
{
  public:
    FPStackingAction();
    virtual ~FPStackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);        
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

