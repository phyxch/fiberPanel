/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

/// Updated: June 3, 2020: Hexc, Zachary and Nadia
///                 Add optical photons following the scheme in loopPanel simulation
///
///                 June 19, 2020: Hexc, Zachary and Nadia
///                 Implementing event generator messenger: i.e. particle gun position (x, y, z)
///
///                 August 5, 2020: Hexc and Zachary
///                 Implementing particle type options:
///                      partileType:   0 - optical photons (default);   1 - muons

#include "FPPrimaryGeneratorAction.hh"
#include "FPPrimaryGeneratorMessenger.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ChargedGeantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPPrimaryGeneratorAction::FPPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
  generatorMessenger = new FPPrimaryGeneratorMessenger(this);
  
  // default particle kinematic

  //
  auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(50.*MeV);

  gunPosition = G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm);
  particleType = 0;   // Optical photon is the default particle type
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPPrimaryGeneratorAction::~FPPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event
  G4ParticleDefinition* particle;
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  //particleTable ->DumpTable("ALL");
  
  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume 
  // from G4LogicalVolumeStore
  //

  G4double xPos, yPos, zPos, xVec, yVec, zVec;
  G4double sigmaAngle, theta, phi, momentum, sigmaMomentum, mass, pp, Ekin;

  if (particleType == 0) {
    //
    // Generate optical photons
    //
    particle = particleTable->FindParticle("opticalphoton");
    fParticleGun->SetParticleDefinition(particle);
    
    G4int npart = 1;
    for (G4int i=1; i<=npart; i++) {
      //Ekin = ( 1.7+(4.8-1.7)*G4UniformRand() )*eV;   // 1.7-4.8eV ~= 200-600 nm
      //Ekin = ( 2.58+(3.1-2.58)*G4UniformRand() )*eV;   // 2.58-3.1eV ~= 400-480 nm
      Ekin = ( 2.034+(4.136-2.034)*G4UniformRand() )*eV;   // 2.58-3.1eV ~= 200-700 nm
      fParticleGun->SetParticleEnergy(Ekin);
      
      fParticleGun->SetParticlePosition(gunPosition); 
      
      theta = G4UniformRand()*180*deg;
      phi = G4UniformRand()*360.*deg;
      yVec = std::sin(phi)*std::sin(theta);
      zVec = std::cos(theta);
      xVec = std::cos(phi)*std::sin(theta);
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(xVec, yVec, zVec));
      //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., -1, 0.));
      
      fParticleGun->GeneratePrimaryVertex(anEvent);
    }
  } else if (particleType == 1) {
    //
    // Generating muons:
    //   Launch one mu- particle for each event
    //
    particle = particleTable->FindParticle("mu-");
    fParticleGun->SetParticleDefinition(particle);
    
    Ekin = ( 2.0+4.0*G4UniformRand() )*GeV;   // muon kinetic energy range: 2 to 6 GeV
    fParticleGun->SetParticleEnergy(Ekin);
      
    fParticleGun->SetParticlePosition(gunPosition); 
      
    theta = G4UniformRand()*45*deg;
    phi = G4UniformRand()*360.*deg;
    yVec = std::sin(phi)*std::sin(theta);
    zVec = -std::cos(theta);
    xVec = std::cos(phi)*std::sin(theta);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(xVec, yVec, zVec));

    G4cout << "Muon KE:  " << Ekin/GeV << " (GeV),   Position (x, y, z) : " << G4BestUnit(gunPosition.getX(), "Length");
    G4cout << "   " << G4BestUnit(gunPosition.getY(), "Length") << "   " << G4BestUnit(gunPosition.getZ(), "Length") << G4endl;
    G4cout << "Direction (xVec, yVec, zVec): " << xVec << ", " << yVec << ", " << zVec << G4endl;
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
  } else {
    // You should not get to here
    G4cout << "Choose a particle type, dummy! " << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
