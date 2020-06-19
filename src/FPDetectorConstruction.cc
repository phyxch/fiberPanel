/// This code was created based on B3a example
/// Date created: May 27, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob

/// Detector geometry description
/// May 27, 2020: Detector system consists of a sheet of plastic scintillator (EJ200 type, 20cm x 20cm x 1cm)
///                         with one embedded wavelengh shifting fiber (20 cm long, 1 mm in diameter) with cladding.
///
//  June 1, 2020: Define scintillator and fiber materials (done!)
//                         Add optical boundary and surfaces.
//
// June 5, 2020: Hexc
//                        Added absorption length for panel and fiber
//

#include "FPDetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SubtractionSolid.hh"
#include "G4OpticalSurface.hh"                // added June 1, 2020
#include "G4LogicalBorderSurface.hh"     // added June 1, 2020
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPDetectorConstruction::FPDetectorConstruction()
: G4VUserDetectorConstruction(),
  fCheckOverlaps(true)
{
  DefineMaterials();
  
  // Define the dimensions of all components 
  panelXY = 20.0*cm;
  panelZ = 1.0*cm;
  fiberD = 1.0*mm;
  fiberL = panelXY;
  grooveL = fiberL;
  grooveD = 1.1*fiberD;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPDetectorConstruction::~FPDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPDetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();
  
  G4bool isotopes = false;
  G4double fractionmass;

  G4Element*  O = man->FindOrBuildElement("O" , isotopes); 
  G4Element* Si = man->FindOrBuildElement("Si", isotopes);
  G4Element* Lu = man->FindOrBuildElement("Lu", isotopes);
  G4Element* H = man->FindOrBuildElement("H",isotopes);
  G4Element*  C = man->FindOrBuildElement("C" , isotopes);   

  G4Material* EJ200_panel = new G4Material("EJ200", 1.023*g/cm3, 2);
  EJ200_panel->AddElement(H, fractionmass=52.43*perCent);
  EJ200_panel->AddElement(C, fractionmass=47.57*perCent);

  G4Material* WLS_Y11_fiber = new G4Material("WLS", 1.05*g/cm3, 2);
  WLS_Y11_fiber->AddElement(H, 8);
  WLS_Y11_fiber->AddElement(C, 8);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* FPDetectorConstruction::Construct()
{  
  // Gamma detector Parameters
  //
  G4double cryst_dX = 6*cm, cryst_dY = 6*cm, cryst_dZ = 3*cm;
  G4int nb_cryst = 32;
  G4int nb_rings = 9;
  //
  G4double dPhi = twopi/nb_cryst, half_dPhi = 0.5*dPhi;
  G4double cosdPhi = std::cos(half_dPhi);
  G4double tandPhi = std::tan(half_dPhi);
  // 
  G4double ring_R1 = 0.5*cryst_dY/tandPhi;
  G4double ring_R2 = (ring_R1+cryst_dZ)/cosdPhi;
  //
  G4double detector_dZ = nb_rings*cryst_dX;
  //
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* panel_mat   = nist->FindOrBuildMaterial("EJ200");
  G4Material* fiber_mat   = nist->FindOrBuildMaterial("WLS");
  G4Material* wrapping_mat = nist->FindOrBuildMaterial("G4_Al");
  
  //     
  // World
  //
  G4double world_sizeXY = 30*cm;
  G4double world_sizeZ  = 5.0*cm;     
  
  G4Box* solidWorld =    
    new G4Box("World",                                                                    //its name
	      0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);   //its size
      
  G4LogicalVolume* WorldLV =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        default_mat,                         //its material
                        "WorldLV");                               //its name
  
  G4VPhysicalVolume* WorldPV = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),             //at (0,0,0)
                      WorldLV,                      //its logical volume
                      "WorldPV",                           //its name
                      0,                                      //its mother  volume
                      false,                                //no boolean operation
                      0,                                      //copy number
                      fCheckOverlaps);             // checking overlaps 
                 
  //
  // Scintillator Panel
  //
  G4Box* solidPanel =    
    new G4Box("Panel",                                             //its name
	      0.5*panelXY, 0.5*panelXY, 0.5*panelZ);   //its size
  
  //     
  // Panel groove
  //
  G4Tubs* solidGroove =
    new G4Tubs("Groove", 0.0, 0.5*grooveD, 0.5*grooveL, 0., twopi);

  
  G4RotationMatrix* yRot = new G4RotationMatrix; 
  yRot->rotateY(90*deg);                                          // rotate 90 degree along Y-axis  
  G4ThreeVector zTrans(0, 0, 0.445*panelZ);          // 0.45 factor causes oversubtraction 

  G4SubtractionSolid* panelGroove =
    new G4SubtractionSolid("Panel-Groove", solidPanel, solidGroove, yRot, zTrans);
  
  
  G4LogicalVolume* PanelLV =                         
    new G4LogicalVolume(panelGroove,           //its solid
                        panel_mat ,                               //its material
                        "PanelLV");                                //its name
  //
  // Place the panel in the world volume
  //
  G4PVPlacement* PanelPV = new G4PVPlacement(0,                 //no rotation
					     G4ThreeVector(),         //at (0,0,0)
					     PanelLV,                   //its logical volume
					     "PanelPV",                    //its name
					     WorldLV,                   //its mother  volume
					     false,                            //no boolean operation
					     0,                                  //copy number
					     fCheckOverlaps);         // checking overlaps
 
  //
  // Wrapping material (Al foil)
  //
  G4double padding_1 = 0.1*mm;   // changed from 0.001*mm to 0.1*mm
  G4double padding_2 = 0.2*mm;   // changed from 0.001*mm to 0.1*mm

  // Smaller box for making the wrapping material
  G4Box* solidWrappingBox_1 =    
    new G4Box("WrappingBox1",                                                                                             //its name
	      0.5*panelXY+padding_1, 0.5*panelXY+padding_1, 0.5*panelZ+padding_1);          //its size

  // Gibber box for making the wrapping material
  G4Box* solidWrappingBox_2 =    
    new G4Box("WrappingBox2",                                                                                             //its name
	      0.5*panelXY+padding_2, 0.5*panelXY+padding_2, 0.5*panelZ+padding_2);          //its size

  G4SubtractionSolid* solidWrapping =
    new G4SubtractionSolid("Wrapping", solidWrappingBox_2, solidWrappingBox_1);
    
  //
  // Making photosensor holes
  //
  
  G4Tubs* solidSensorHole =
    new G4Tubs("Hole", 0.0, 0.8*fiberD, 0.5*(padding_2 - padding_1), 0., twopi); // wider than the groove diameter.

  // Rotate along y-axis (defined earlier for making the groove
  //  yRot->rotateY(90*deg);                                          // rotate 90 degree along Y-axis  
  G4ThreeVector holeTrans( (0.5*panelXY+padding_1+0.5*(padding_2-padding_1)), 0, 0.445*panelZ);  

  G4SubtractionSolid* solidWrappingHole =
    new G4SubtractionSolid("WrappingHole", solidWrapping, solidSensorHole, yRot, holeTrans);

  G4LogicalVolume* WrappingLV =
    new G4LogicalVolume(solidWrappingHole,
			wrapping_mat,
			"WrappingLV");

  G4PVPlacement* WrappingPV = new G4PVPlacement(0,                 //no rotation
					     G4ThreeVector(),         //at (0,0,0)
					     WrappingLV,                //its logical volume
					     "WrappingPV",             //its name
					     WorldLV,                      //its mother  volume
					     false,                            //no boolean operation
					     0,                                  //copy number
					     fCheckOverlaps);         // checking overlaps
  
  
  //     
  // Wavelength shifting fiber
  //

  G4Tubs* solidFiber =
    new G4Tubs("Fiber", 0.0, 0.5*fiberD, 0.5*fiberL, 0., twopi);
      
  G4LogicalVolume* FiberLV =                         
    new G4LogicalVolume(solidFiber,       //its solid
                        fiber_mat,                     //its material
                        "FiberLV");                        //its name
                                 
  // 
  // Place the fiber at the center of the groove
  //
  //G4double phi = icrys*dPhi;
  G4RotationMatrix *rotm  = new G4RotationMatrix();
  rotm->rotateY(90*deg); 
  //rotm.rotateZ(phi);
  //G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);     
  //G4ThreeVector position = (ring_R1+0.5*cryst_dZ)*uz;
  //G4Transform3D transform = G4Transform3D(rotm,position);

  G4PVPlacement* FiberPV = new G4PVPlacement(rotm,                       //no rotation
					     G4ThreeVector(0.0, 0.0, 0.445*panelZ),               //at (0,0,0)
					     FiberLV,                         //its logical volume
					     "FiberPV",                         //its name
					     WorldLV,                       //its mother  volume
					     false,                                //no boolean operation
					     0,                                      //copy number
					     fCheckOverlaps);             // checking overlaps 
  
  // Visualization attributes
  //
  WorldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  //PanelLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  //FiberLV->SetVisAttributes (G4VisAttributes::GetInvisible());    

  //
  // Define optical material properties for the air and panel
  //
  const G4int NUM = 2;
  G4double Ephoton[NUM] = {2.00*eV, 3.47*eV};   // TO match the photon energy range as defined in the gdml file
  G4double RefractiveIndex_Panel[NUM] = {1.58, 1.58};
  G4double RefractiveIndex_Air[NUM] = {1.0, 1.0};

  G4double Absorption_Fiber[NUM] = {5.40*m, 5.40*m};
  G4double Absorption_Panel[NUM] = {4.0*m, 4.0*m};
  G4double Absorption_Air[NUM] = {10.0*m, 10.0*m};    // A guessed number
    
  //  G4double SpecularLobe[NUM]    = {0.3, 0.3};
  //  G4double SpecularSpike[NUM]   = {0.2, 0.2};
  //  G4double Backscatter[NUM]     = {0.2, 0.2};       // need to check these values
  
  // Adjusted for scintillator panel
  G4double PanelReflect[NUM]      = {0.95, 0.95};     // changed from 0.95, 0.95 to 0.85, 0.98
  G4double AirReflect[NUM]      = {0.98, 0.98}; 

  /* Copied from WLS example  6/19/2020  */

  G4double photonEnergy[] =
  {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
   2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
   2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
   2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
   2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
   2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
   2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
   3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
   3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
   3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};

  
  const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);
  
  G4double refractiveIndexWLSfiber[] =
  { 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60};

  assert(sizeof(refractiveIndexWLSfiber) == sizeof(photonEnergy));

  G4double absWLSfiber[] =
  {5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
   5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
   5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,1.10*m,
   1.10*m,1.10*m,1.10*m,1.10*m,1.10*m,1.10*m, 1.*mm, 1.*mm, 1.*mm, 1.*mm,
    1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm};

  assert(sizeof(absWLSfiber) == sizeof(photonEnergy));

  G4double emissionFib[] =
  {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
   3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
   12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
   15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
   0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

  assert(sizeof(emissionFib) == sizeof(photonEnergy));

  // Add entries into properties table
  G4MaterialPropertiesTable* mptWLSfiber = new G4MaterialPropertiesTable();
  mptWLSfiber->
           AddProperty("RINDEX",photonEnergy,refractiveIndexWLSfiber,nEntries);
  // mptWLSfiber->AddProperty("ABSLENGTH",photonEnergy,absWLSfiber,nEntries);
  mptWLSfiber->AddProperty("WLSABSLENGTH",photonEnergy,absWLSfiber,nEntries);
  mptWLSfiber->AddProperty("WLSCOMPONENT",photonEnergy,emissionFib,nEntries);
  mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);

  //  fPMMA->SetMaterialPropertiesTable(mptWLSfiber);

  /*
  G4MaterialPropertiesTable *Fiber = new G4MaterialPropertiesTable();
  Fiber->AddProperty("RINDEX", Ephoton, RefractiveIndex_Panel, NUM);    // Use the same for panel
  Fiber->AddProperty("REFLECTIVITY", Ephoton, PanelReflect, NUM);          // Use the same for panel
  Fiber->AddProperty("WLSABSLENGTH", Ephoton, Absorption_Fiber, NUM);
  Fiber->AddProperty("ABSLENGTH", Ephoton, Absorption_Fiber, NUM);
  //Fiber->AddProperty("WLSCOMPONENT", Ephoton, EmissionWLS, NUM);      
  */
  
  G4MaterialPropertiesTable *Panel = new G4MaterialPropertiesTable();
  Panel->AddProperty("RINDEX", Ephoton, RefractiveIndex_Panel, NUM);
  Panel->AddProperty("REFLECTIVITY", Ephoton, PanelReflect, NUM);
  Panel->AddProperty("ABSLENGTH", Ephoton, Absorption_Panel, NUM);
  
  G4MaterialPropertiesTable *Air = new G4MaterialPropertiesTable();
  Air->AddProperty("RINDEX", Ephoton, RefractiveIndex_Air, NUM);
  Air->AddProperty("REFLECTIVITY", Ephoton, AirReflect, NUM);
  Air->AddProperty("ABSLENGTH", Ephoton, Absorption_Air, NUM);
  
  // Optical surfaces and boundaries
  //
  // For Panel
  G4OpticalSurface *OpticalPanel = new G4OpticalSurface("PanelSurface");
  OpticalPanel->SetModel(unified);
  OpticalPanel->SetType(dielectric_dielectric);
  //OpticalPanel->SetFinish(polishedfrontpainted);    // polishdfrontpainted: only reflection, absorption and no refraction
  //  OpticalPanel->SetFinish(polished);    // polishd: follows Snell's law
  OpticalPanel->SetFinish(ground);    // ground: follows Snell's law
  OpticalPanel->SetMaterialPropertiesTable(Panel);
  /*
  GROUND:The incidence of a photon upon a rough surface requires choosing the angle, a, between a ‘micro-facet’ normal and that of the average surface. 
The UNIFIED model assumes that the probability of micro-facet normals populates the annulus of solid angle sin(a)dawill be  proportional to a gaussian of SigmaAlpha:theOpSurface->SetSigmaAlpha(0.1);where sigma_alpha is in [rad]
In the GLISUR model this is indicated by the value of polish; when it is <1, then a random point is generated in a sphere of radius (1-polish), and the corresponding vector is added to the normal. The value 0 means maximum roughness with effective plane of reflection distributedas cos(a).theOpSurface -> SetPolish (0.0)
  */

  // For Fiber
  G4OpticalSurface *OpticalFiber = new G4OpticalSurface("FiberSurface");
  OpticalFiber->SetModel(unified);
  OpticalFiber->SetType(dielectric_dielectric);
  OpticalFiber->SetFinish(polished);    // polishdfronpainted: only reflection, absorption and no refraction
  //   OpticalAirFiber->SetFinish(polished);    // polished: follows Snell's law
  //  OpticalFiber->SetMaterialPropertiesTable(Fiber);
  OpticalFiber->SetMaterialPropertiesTable(mptWLSfiber);

  // For Air
  G4OpticalSurface *OpticalAir = new G4OpticalSurface("AirSurface");
  OpticalAir->SetModel(unified);
  OpticalAir->SetType(dielectric_dielectric);
  OpticalAir->SetFinish(polished);    // polishdfronpainted: only reflection, absorption and no refraction
  //   OpticalAirFiber->SetFinish(polished);    // polished: follows Snell's law
  OpticalAir->SetMaterialPropertiesTable(Air);

  // Set the material properties for the optics
  panel_mat->SetMaterialPropertiesTable(Panel);
  // fiber_mat->SetMaterialPropertiesTable(Fiber);
  fiber_mat->SetMaterialPropertiesTable(mptWLSfiber);
  default_mat->SetMaterialPropertiesTable(Air);    // Air
  
  // Optical border surface
  //
  new G4LogicalBorderSurface("Panel/Air", PanelPV,  WorldPV, OpticalPanel);
  new G4LogicalBorderSurface("Air/Panel", WorldPV,  PanelPV, OpticalPanel);

  new G4LogicalBorderSurface("Air/Fiber",  WorldPV, FiberPV,  OpticalFiber);
  new G4LogicalBorderSurface("Fiber/Air",  FiberPV, WorldPV,  OpticalFiber);   

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl; 

  //always return the physical World
  //
  return WorldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FPDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  
  /*  
  // declare crystal as a MultiFunctionalDetector scorer
  //  
  G4MultiFunctionalDetector* cryst = new G4MultiFunctionalDetector("crystal");
  G4SDManager::GetSDMpointer()->AddNewDetector(cryst);
  G4VPrimitiveScorer* primitiv1 = new G4PSEnergyDeposit("edep");
  cryst->RegisterPrimitive(primitiv1);
  SetSensitiveDetector("CrystalLV",cryst);
  
  // declare patient as a MultiFunctionalDetector scorer
  //  
  G4MultiFunctionalDetector* patient = new G4MultiFunctionalDetector("patient");
  G4SDManager::GetSDMpointer()->AddNewDetector(patient);
  G4VPrimitiveScorer* primitiv2 = new G4PSDoseDeposit("dose");
  patient->RegisterPrimitive(primitiv2);
  SetSensitiveDetector("PatientLV",patient);
  */
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
