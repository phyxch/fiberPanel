/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

/// Updated: June 3, 2020: Hexc, Zachary and Nadia
///                 Add optical photons following the scheme in loopPanel simulation
///
///

#include "FPPrimaryGeneratorAction.hh"

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPPrimaryGeneratorAction::FPPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic

  //
  auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(50.*MeV);

  /*
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle
                    = particleTable->FindParticle("chargedgeantino");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
  fParticleGun->SetParticleEnergy(1*eV);    
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
  */
  
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
/*
  G4double panelXHalfLength = 0., panelYHalfLength = 0., panelZHalfLength = 0.;
  auto panelLV = G4LogicalVolumeStore::GetInstance()->GetVolume("PanelLV");
  
  // Check that the panel volume has box shape
  G4Box* panelBox = nullptr;
  
  if (  panelLV ) {
  //G4cout << "We found the panel! " << G4endl;
    panelBox = dynamic_cast<G4Box*>(panelLV->GetSolid());
    }    
  
  if ( panelBox ) {
  panelXHalfLength = panelBox->GetXHalfLength();
    panelYHalfLength = panelBox->GetYHalfLength();
    panelZHalfLength = panelBox->GetZHalfLength();  
    //	G4cout << " panelZHalfLength = " << panelZHalfLength << G4endl;
    }
  else  {
  G4ExceptionDescription msg;
    msg << "Panel volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("FPPrimaryGeneratorAction::GeneratePrimaries()",
    "MyCode0002", JustWarning, msg);
		} 
*/

  G4double xPos, yPos, zPos, xVec, yVec, zVec;
  G4double sigmaAngle, theta, phi, momentum, sigmaMomentum, mass, pp, Ekin;
  G4int pid;

  pid = 3;   // tmp pID
  particle = particleTable->FindParticle("opticalphoton");
  fParticleGun->SetParticleDefinition(particle);
  
  G4int npart = 1;
  for (G4int i=1; i<=npart; i++) {
    //		Ekin = ( 1.7+(4.8-1.7)*G4UniformRand() )*eV;   // 1.7-4.8eV ~= 200-600 nm
    //Ekin = ( 2.58+(3.1-2.58)*G4UniformRand() )*eV;   // 2.58-3.1eV ~= 400-480 nm
    Ekin = ( 2.034+(4.136-2.034)*G4UniformRand() )*eV;   // 2.58-3.1eV ~= 200-700 nm
    fParticleGun->SetParticleEnergy(Ekin);
    /*
      yPos = (G4UniformRand()-0.5)*200.0*cm;
      xPos = (G4UniformRand()-0.5)*200.0*cm;
    */
    yPos = 0.0*10*cm;    
    xPos = 0.0*10*cm; 
    zPos = 0.8*5*mm;      // inside fiber
    //zPos = 0.5*5*mm;      // inside panel
    fParticleGun->SetParticlePosition(G4ThreeVector(xPos, yPos, zPos)); 
    
    theta = G4UniformRand()*180*deg;
    //theta = 90.0*deg;
    phi = G4UniformRand()*360.*deg;
    yVec = std::sin(phi)*std::sin(theta);
    zVec = std::cos(theta);
    xVec = std::cos(phi)*std::sin(theta);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(xVec, yVec, zVec));
    //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., -1, 0.));
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
    
    /*  This section of the code is from the example 3Ba 
	G4ParticleDefinition* particle = fParticleGun->GetParticleDefinition();
	if (particle == G4ChargedGeantino::ChargedGeantino()) {
	//fluorine 
	G4int Z = 9, A = 18;
	G4double ionCharge   = 0.*eplus;
	G4double excitEnergy = 0.*keV;
	
	G4ParticleDefinition* ion
	= G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
	fParticleGun->SetParticleDefinition(ion);
	fParticleGun->SetParticleCharge(ionCharge);
	}
	
	// randomized position
	//
	///G4double x0  = 0*cm, y0  = 0*cm, z0  = 0*cm;
	///G4double dx0 = 0*cm, dy0 = 0*cm, dz0 = 0*cm;   
	G4double x0  = 4*cm, y0  = 4*cm, z0  = 4*cm;
	G4double dx0 = 1*cm, dy0 = 1*cm, dz0 = 1*cm; 
	x0 += dx0*(G4UniformRand()-0.5);
	y0 += dy0*(G4UniformRand()-0.5);
	z0 += dz0*(G4UniformRand()-0.5);
	fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
	
	//create vertex
	//
	fParticleGun->GeneratePrimaryVertex(anEvent);
    */
  }
}  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
