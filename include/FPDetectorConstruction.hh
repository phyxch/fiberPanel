/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

#ifndef FPDetectorConstruction_h
#define FPDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.
///
/// Crystals are positioned in Ring, with an appropriate rotation matrix. 
/// Several copies of Ring are placed in the full detector.

class FPDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  FPDetectorConstruction();
  virtual ~FPDetectorConstruction();
  
public:
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
  
private:
  void DefineMaterials();
  
  G4double panelXY, panelZ;
  G4double fiberD, fiberL;               // diameter and length
  G4double grooveD, grooveL;        // 
  
  G4bool  fCheckOverlaps;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
