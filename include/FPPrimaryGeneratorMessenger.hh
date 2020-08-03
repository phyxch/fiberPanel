/// June 19, 2020: Hexc, Zachary and Nadia
///                 Implementing event generator messenger: i.e. particle gun position (x, y, z)
///
/// August 3, 2020: Hexc, Zachary and Nadia
///                 Implementing particle type choices:  0 for optical photons; 1 for muons.
///

#ifndef FPPrimaryGeneratorMessenger_h
#define FPPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class FPPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class FPPrimaryGeneratorMessenger: public G4UImessenger
{
public:
  FPPrimaryGeneratorMessenger(FPPrimaryGeneratorAction*);
  ~FPPrimaryGeneratorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  
private:
  FPPrimaryGeneratorAction*      FPAction;
  G4UIdirectory*                           gunDir; 
  G4UIcmdWithAnInteger*           SetGunParticleType;
  G4UIcmdWith3VectorAndUnit*  SetGunPositionCmd;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
