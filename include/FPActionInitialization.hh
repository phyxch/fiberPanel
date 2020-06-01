/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

#ifndef FPActionInitialization_h
#define FPActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.
///

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class FPActionInitialization : public G4VUserActionInitialization
{
  public:
    FPActionInitialization();
    virtual ~FPActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
