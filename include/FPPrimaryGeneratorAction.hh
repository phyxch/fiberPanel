/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Updated: June 3, 2020: hexc, Zachary and Nadia
///                 Removed meaningless comments.
///
///                 June 19, 2020: Hexc, Zachary and Nadia
///                 Implementing event generator messenger: i.e. particle gun position (x, y, z)
///

#ifndef FPPrimaryGeneratorAction_h
#define FPPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4UIcmdWith3VectorAndUnit;
class FPPrimaryGeneratorMessenger;

class FPPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    FPPrimaryGeneratorAction();    
  virtual ~FPPrimaryGeneratorAction();
  
  virtual void GeneratePrimaries(G4Event*);         
  
  const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  inline void SetGunPosition(G4ThreeVector aVec){gunPosition = aVec;}
  inline void SetGunParticleType(G4int  nType){particleType = nType;}

private:
  G4ParticleGun*  fParticleGun;
  FPPrimaryGeneratorMessenger* generatorMessenger;
  G4ThreeVector  gunPosition;
  G4int particleType;   // 0: optical photon, 1: muons 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
