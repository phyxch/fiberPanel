// June 19, 2020: Hexc, Zachary and Nadia
///                 Implementing event generator messenger: i.e. particle gun position (x, y, z)
///           
/// August 3, 2020: Hexc, Zachary and Nadia
///                 Implementing particle type choices:  0 for optical photons; 1 for muons.
///

#include "globals.hh"

#include "FPPrimaryGeneratorMessenger.hh"

#include "FPPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "CLHEP/Units/SystemOfUnits.h"

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPPrimaryGeneratorMessenger::FPPrimaryGeneratorMessenger(
                                          FPPrimaryGeneratorAction* FPGun)
:FPAction(FPGun)
{
  gunDir = new G4UIdirectory("/FP/gun/");
  gunDir->SetGuidance("PrimaryGenerator event control:");

  SetGunPositionCmd =
    new G4UIcmdWith3VectorAndUnit("/FP/gun/position", this);
  SetGunPositionCmd->SetGuidance("Set particle gun position");
  SetGunPositionCmd->SetParameterName("X","Y","Z", true);
  SetGunPositionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  SetGunPositionCmd->SetUnitCategory("Length");
  
  // New event type message
  SetGunParticleType = new G4UIcmdWithAnInteger("/FP/gun/particleType", this);
  SetGunParticleType->SetGuidance("Set particle type");
  SetGunParticleType->SetGuidance("       Choice :  0, 1");
  SetGunParticleType->SetParameterName("particleType", true);
  SetGunParticleType->SetDefaultValue(0);
  SetGunParticleType->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPPrimaryGeneratorMessenger::~FPPrimaryGeneratorMessenger()
{
  delete SetGunPositionCmd;
  delete SetGunParticleType;
  delete gunDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPPrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValues)
{ 
    if (command == SetGunPositionCmd ) {
      G4ThreeVector vec = SetGunPositionCmd->GetNew3VectorValue(newValues);
      FPAction->SetGunPosition(vec);
    }

    if (command == SetGunParticleType ) {
      G4int particleType = SetGunParticleType->GetNewIntValue(newValues);
      FPAction->SetGunParticleType(particleType);
    }  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

