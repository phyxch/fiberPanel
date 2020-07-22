/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// July 10, 2020: Hexc, Nadia, Zachary
///                        Redefine the data members of the detector components.
///                        including material types
/// 

#ifndef FPDetectorConstruction_h
#define FPDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

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
  //  G4double grooveD, grooveL;        //
  G4double claddingD, claddingL;
  G4double epoxyD, epoxyL;
  
  G4Material *panel_mat, *fiber_mat, *cladding_mat, *epoxy_mat;
  G4Material *default_mat, *wrapping_mat;

  G4LogicalVolume* sipmLV;
  
  G4bool  fCheckOverlaps;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
