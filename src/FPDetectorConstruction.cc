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
// June 19, 2020: Hexc, Nadia and Zachary
//                        Added the optical property parameters for fiber following the GEANT4 example WLS.
//
// July 1, 2020: Hexc, Nadia and Zachary
//                        Implemented SiPM photosensor detector volume (sensitive volume). We also had an alumunum wrapping with a
//                        square hole where the SiPM is installed.
//
// July 10, 2020: Hexc, Nadia and Zachary
//                        Update the detector components construction in the following hierarchy
//                        World <-  Aluminum wrapping <- Panel <- glue <- cladding <- fiber core
//
// August 3, 2020: Hexc, Nadia and Zachary
//                        Add scintilation parameters for the Panel (i.e., EJ-200 scintillator).
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
#include "G4LogicalSkinSurface.hh"         // added June 22, 2020
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"        // added July 1, 2020
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "FPSiPMSD.hh"                           // added July 22, 2020

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPDetectorConstruction::FPDetectorConstruction()
: G4VUserDetectorConstruction(),
  fCheckOverlaps(true)
{
  DefineMaterials();
  
  // Define the dimensions of all components 
  panelXY = 20.0*cm;
  panelZ = 1.0*cm;

  // For Kuaray Y-11 fiber
  claddingD = 1.0*mm;
  claddingL = panelXY-0.01*mm;
  //grooveL = fiberL;
  //grooveD = 1.01*fiberD;

  fiberL = claddingL-0.01*mm;
  fiberD = claddingD-0.06*mm;

  epoxyL = panelXY - 0.005*mm;
  epoxyD = 1.1*claddingD;
 
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
  G4Element* Cl = man->FindOrBuildElement("Cl", isotopes);
  G4Element* N = man->FindOrBuildElement("N", isotopes);
  G4Element* H = man->FindOrBuildElement("H",isotopes);
  G4Element*  C = man->FindOrBuildElement("C" , isotopes);

  G4Material* EJ200_panel = new G4Material("EJ200", 1.023*g/cm3, 2);
  EJ200_panel->AddElement(H, fractionmass=52.43*perCent);
  EJ200_panel->AddElement(C, fractionmass=47.57*perCent);

  // Optical Cement: EJ-500
  G4Material* EJ500_epoxy  = new G4Material("EJ500", 1.13*g/cm3, 5);  // from similar material
  EJ500_epoxy->AddElement(H, 49);
  EJ500_epoxy->AddElement(C, 31);
  EJ500_epoxy->AddElement(O, 6);
  EJ500_epoxy->AddElement(N, 2);
  EJ500_epoxy->AddElement(Cl, 1);
  
  G4Material* WLS_Y11_fiber = new G4Material("WLS", 1.05*g/cm3, 2);
  WLS_Y11_fiber->AddElement(H, 8);
  WLS_Y11_fiber->AddElement(C, 8);

  // Fiber cladding: PMMA
  G4Material* WLS_Y11_cladding = new G4Material("Cladding", 1.19*g/cm3, 3);
  WLS_Y11_cladding->AddElement(C, fractionmass=33.65*perCent );
  WLS_Y11_cladding->AddElement(H, fractionmass=53.27*perCent );
  WLS_Y11_cladding->AddElement(O, fractionmass=13.08*perCent );

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
  default_mat = nist->FindOrBuildMaterial("G4_AIR");
  panel_mat   = nist->FindOrBuildMaterial("EJ200");
  fiber_mat   = nist->FindOrBuildMaterial("WLS");
  cladding_mat   = nist->FindOrBuildMaterial("Cladding");
  epoxy_mat   = nist->FindOrBuildMaterial("EJ500");
  wrapping_mat = nist->FindOrBuildMaterial("G4_Al");
  
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
                      WorldLV,                          //its logical volume
                      "WorldPV",                       //its name
                      0,                                      //its mother  volume
                      false,                                //no boolean operation
                      0,                                      //copy number
                      fCheckOverlaps);             // checking overlaps 
                 
  //
  // Scintillator Panel
  //
  G4Box* solidPanel =    
    new G4Box("Panel",
	      0.5*panelXY, 0.5*panelXY, 0.5*panelZ);
  
  G4LogicalVolume* PanelLV =                         
    new G4LogicalVolume(solidPanel, 
                        panel_mat ,
                        "PanelLV");
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

  // Bigger box for making the wrapping material
  G4Box* solidWrappingBox_2 =    
    new G4Box("WrappingBox2",                                                                                             //its name
	      0.5*panelXY+padding_2, 0.5*panelXY+padding_2, 0.5*panelZ+padding_2);          //its size

  G4SubtractionSolid* solidWrapping =
    new G4SubtractionSolid("Wrapping", solidWrappingBox_2, solidWrappingBox_1);
    
  //
  // Making photosensor holes
  //

  // Replace the circular opening with a square hole
  //  G4Tubs* solidSensorHole =
  //  new G4Tubs("Hole", 0.0, 0.8*fiberD, 0.5*(padding_2 - padding_1), 0., twopi); // wider than the groove diameter.
  // 
  G4double SiPM_x = padding_2 - padding_1;
  G4double SiPM_y = 1.09*mm;
  G4double SiPM_z = 1.09*mm;

  G4Box* solidSensor = new G4Box("Hole", SiPM_x/2, SiPM_y/2, SiPM_z/2);
  sipmLV = new G4LogicalVolume(solidSensor,
			       default_mat,
			       "sipmLV");
  
  G4PVPlacement* sipmPV = new G4PVPlacement(0,                 //no rotation
					     G4ThreeVector( (0.5*panelXY+padding_1+0.5*(padding_2-padding_1)), 0, 0.445*panelZ ),         //at (0,0,0)
					     sipmLV,                        //its logical volume
					     "sipmPV",                     //its name
					     WorldLV,                      //its mother  volume
					     false,                            //no boolean operation
					     0,                                  //copy number
					     fCheckOverlaps);         // checking overlaps

  // SiPM sensor visualization attribute
  G4VisAttributes photonDetectorVisAtt(G4Colour::Red());
  photonDetectorVisAtt.SetForceWireframe(true);
  photonDetectorVisAtt.SetLineWidth(3.);
  sipmLV->SetVisAttributes(photonDetectorVisAtt);  

  // Create a opening hole in the wrapping for installing sensor 
  G4double Hole_x = padding_2 - padding_1;
  G4double Hole_y = 1.1*mm;
  G4double Hole_z = 1.1*mm;
  
  G4Box* solidSensorHole = new G4Box("Hole", Hole_x/2, Hole_y/2, Hole_z/2);
  
  // Rotate along y-axis (defined earlier for making the groove
  //  yRot->rotateY(90*deg);                                          // rotate 90 degree along Y-axis  
  G4ThreeVector holeTrans( (0.5*panelXY+padding_1+0.5*(padding_2-padding_1)), 0, 0.445*panelZ);  

  G4SubtractionSolid* solidWrappingHole =
    new G4SubtractionSolid("WrappingHole", solidWrapping, solidSensorHole, 0, holeTrans);

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
  // Optical epoxy
  //
  G4Tubs* solidEpoxy =
    new G4Tubs("Epoxy", 0.0, 0.5*epoxyD, 0.5*epoxyL, 0., twopi);
  
  G4LogicalVolume* EpoxyLV =                         
    new G4LogicalVolume(solidEpoxy,       //its solid
                        epoxy_mat,                          //its material
                        "EpoxyLV");                        //its name
  
  G4RotationMatrix* yRot = new G4RotationMatrix; 
  yRot->rotateY(90*deg);                                          // rotate 90 degree along Y-axis  

  // Put epoxy inside the Panel
  G4PVPlacement* EpoxyPV = new G4PVPlacement(yRot,                                        // no rotation
					     G4ThreeVector(0.0, 0.0, 0.5*(panelZ - epoxyD)),         // Near the surface of the panel
					     EpoxyLV,                                                                         //its logical volume
					     "EpoxyPV",                                                                      //its name
					     PanelLV,                           // its mother  volume
					     false,                                // no boolean operation
					     0,                                      // copy number
					    fCheckOverlaps);             // checking overlaps 
  //     
  // Y-11 cladding
  //
  G4Tubs* solidCladding =
    new G4Tubs("Cladding", 0.0, 0.5*claddingD, 0.5*claddingL, 0., twopi);
  
  G4LogicalVolume* CladdingLV =                         
    new G4LogicalVolume(solidCladding,       //its solid
                        cladding_mat,        //its material
                        "CladdingLV");       //its name
  
  // Put cladding inside the Epoxy
  G4PVPlacement* CladdingPV = new G4PVPlacement(0,                                       
					     G4ThreeVector(0.0, 0.0, 0.0),        
					     CladdingLV,                                     
					     "CladdingPV",    
					     EpoxyLV, 
					     false,   
					     0,
					     fCheckOverlaps);
  //     
  // Wavelength shifting fiber
  //
  G4Tubs* solidFiber =
    new G4Tubs("Fiber", 0.0, 0.5*fiberD, 0.5*fiberL, 0., twopi);
  
  G4LogicalVolume* FiberLV =                         
    new G4LogicalVolume(solidFiber,
                        fiber_mat,
                        "FiberLV");                                                               
  // 
  // Place the fiber at the center of the cladding
  //
  
  G4PVPlacement* FiberPV = new G4PVPlacement(0,
					     G4ThreeVector(0.0, 0.0, 0.0),
					     FiberLV, 
					     "FiberPV", 
					     CladdingLV,
					     false, 
					     0,
					     fCheckOverlaps);
  
  // Visualization attributes
  //
  WorldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  //PanelLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  //FiberLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  G4VisAttributes* cladding = new G4VisAttributes(G4Colour(0.3, 0.8, 0.2));
  cladding->SetVisibility(true);
  cladding->SetForceWireframe(false);
  cladding->SetForceSolid(true);
  
  CladdingLV->SetVisAttributes(cladding);  

  //
  // Define optical material properties for panel
  //
  // We use EJ-200 plastic scintillator for this study. The optical properties can be found from the link below:
  // https://eljentechnology.com/products/plastic-scintillators/ej-200-ej-204-ej-208-ej-212
  //

  //  G4double photonEnergy[] =
  std::vector<G4double> photonEnergy=
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
  
  std::vector<G4double> RefractiveIndex_Panel =                                              // from EJEN website
    {1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
     1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
     1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
     1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
     1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58};
  
  assert(sizeof(RefractiveIndex_Panel) == sizeof(photonEnergy));
  
  std::vector<G4double> Absorption_Panel =                                                      // from EJEN website
    {3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m,
     3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m,
     3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m,
     3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m,
     3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m, 3.8*m
    };
  
  assert(sizeof(Absorption_Panel) == sizeof(photonEnergy));

  // Adjusted for scintillator panel
  std::vector<G4double> PanelReflect =
    {0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95,
     0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95,
     0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95,
     0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95,
     0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95
    };

  assert(sizeof(PanelReflect) == sizeof(photonEnergy));

  // Not sure about these parameters yet!!!  6/22/2020
  //  G4double SpecularLobe[NUM]    = {0.3, 0.3};
  //  G4double SpecularSpike[NUM]   = {0.2, 0.2};
  //  G4double Backscatter[NUM]     = {0.2, 0.2};       // need to check these values
  
  G4MaterialPropertiesTable *Panel = new G4MaterialPropertiesTable();
  //  Panel->AddProperty("RINDEX", photonEnergy, RefractiveIndex_Panel, nEntries);
  //  Panel->AddProperty("REFLECTIVITY", photonEnergy, PanelReflect, nEntries);
  //  Panel->AddProperty("ABSLENGTH", photonEnergy, Absorption_Panel, nEntries);
  Panel->AddProperty("RINDEX", photonEnergy, RefractiveIndex_Panel);
  Panel->AddProperty("REFLECTIVITY", photonEnergy, PanelReflect);
  Panel->AddProperty("ABSLENGTH", photonEnergy, Absorption_Panel);  
  
  // For EJ-200 scintillator
  //G4double ScintEnergy[] = {2.48*eV, 3.1*eV};
  //G4double ScintFast[] = {1.0, 1.1};
  //const G4int nScintEntries = sizeof(ScintEnergy)/sizeof(G4double);
  //  Panel->AddProperty("FASTCOMPONENT", ScintEnergy, ScintFast, nScintEntries);
  std::vector<G4double> ScintFast =
    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  
  Panel->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, ScintFast);
  
  Panel->AddConstProperty("SCINTILLATIONYIELD", 10000/MeV);
  Panel->AddConstProperty("RESOLUTIONSCALE", 1.);
  //  Panel->AddConstProperty("FASTTIMECONSTANT", 1.*ns);
  Panel->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1.*ns);
  Panel->AddConstProperty("SCINTILLATIONYIELD1", 1.);
  
  // set in the panel material property
  panel_mat->SetMaterialPropertiesTable(Panel);

  //
  // Define optical material properties for air
  //
  std::vector<G4double> RefractiveIndex_Air =
    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  
  assert(sizeof(RefractiveIndex_Air) == sizeof(photonEnergy));
  
  std::vector<G4double> Absorption_Air =
    {10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m,
     10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m,
     10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m,
     10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m,
     10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m, 10.0*m
    };    // A guessed number

  assert(sizeof(Absorption_Air) == sizeof(photonEnergy));

  // Need to check this parameter for air  6/22/2020
  std::vector<G4double> AirReflect      =
    {0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98,
     0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98,
     0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98,
     0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98,
     0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98
    }; 
  
  assert(sizeof(AirReflect) == sizeof(photonEnergy));
  
  G4MaterialPropertiesTable *Air = new G4MaterialPropertiesTable();
  Air->AddProperty("RINDEX", photonEnergy, RefractiveIndex_Air);
  // Note 100% sure if one needs to define the following two properties or not.
  //  Air->AddProperty("REFLECTIVITY", photonEnergy, AirReflect, nEntries);
  //  Air->AddProperty("ABSLENGTH", photonEnergy, Absorption_Air, nEntries);

  default_mat->SetMaterialPropertiesTable(Air);  
  
  // For Expoxy (EJ-500)
  std::vector<G4double> refractiveIndexEpoxy =
  { 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57,
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57,
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57,
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57,
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57};

  assert(sizeof(refractiveIndexEpoxy) == sizeof(photonEnergy));

  G4MaterialPropertiesTable *Epoxy = new G4MaterialPropertiesTable();
  Epoxy->AddProperty("RINDEX", photonEnergy, refractiveIndexEpoxy);
  Epoxy->AddProperty("REFLECTIVITY", photonEnergy, PanelReflect);    // same as for panel. May not need!
  Epoxy->AddProperty("ABSLENGTH", photonEnergy, Absorption_Panel);     // same as for panel  for now !

  epoxy_mat->SetMaterialPropertiesTable(Epoxy);
  
  // For Y11-Cladding
  std::vector<G4double> refractiveIndexCladding =
  { 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49};

  assert(sizeof(refractiveIndexCladding) == sizeof(photonEnergy));

  G4MaterialPropertiesTable *Cladding = new G4MaterialPropertiesTable();
  Cladding->AddProperty("RINDEX", photonEnergy, refractiveIndexCladding);
  Cladding->AddProperty("REFLECTIVITY", photonEnergy, PanelReflect);          // same as for panel. May not need!
  Cladding->AddProperty("ABSLENGTH", photonEnergy, Absorption_Panel);     // same as panel  for now !

  cladding_mat->SetMaterialPropertiesTable(Cladding);

  //
  // Define optical material properties for the fiber
  /* Copied from WLS example  6/19/2020  */
  
  std::vector<G4double> refractiveIndexWLSfiber =
    { 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
      1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
      1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
      1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
      1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60};
  
  assert(sizeof(refractiveIndexWLSfiber) == sizeof(photonEnergy));
  
  std::vector<G4double> absWLSfiber =
    {5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
     5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
     5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,1.10*m,
     1.10*m,1.10*m,1.10*m,1.10*m,1.10*m,1.10*m, 1.*mm, 1.*mm, 1.*mm, 1.*mm,
     1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm};
  
  assert(sizeof(absWLSfiber) == sizeof(photonEnergy));
  
  std::vector<G4double> emissionFib =
    {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
     3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
     12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
     15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
     0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};
  
  assert(sizeof(emissionFib) == sizeof(photonEnergy));
  
  // Add entries into properties table
  G4MaterialPropertiesTable* mptWLSfiber = new G4MaterialPropertiesTable();
  mptWLSfiber->AddProperty("RINDEX",photonEnergy,refractiveIndexWLSfiber);
  mptWLSfiber->AddProperty("WLSABSLENGTH",photonEnergy,absWLSfiber);
  mptWLSfiber->AddProperty("WLSCOMPONENT",photonEnergy,emissionFib);
  mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);

  //
  // Set the material properties for the optics
  //
  fiber_mat->SetMaterialPropertiesTable(mptWLSfiber);

  
  //
  // Optical surfaces and boundaries
  //
  // For Panel
  G4OpticalSurface *OpticalPanel = new G4OpticalSurface("PanelSurface");
  OpticalPanel->SetModel(unified);
  OpticalPanel->SetType(dielectric_dielectric);
  //OpticalPanel->SetFinish(polishedfrontpainted);    // polishdfrontpainted: only reflection, absorption and no refraction
  //OpticalPanel->SetFinish(polished);    // polishd: follows Snell's law
  OpticalPanel->SetFinish(ground);    // ground: follows Snell's law
  
  // For Epoxy
  G4OpticalSurface *OpticalEpoxy = new G4OpticalSurface("EpoxySurface");
  OpticalEpoxy->SetModel(unified);
  OpticalEpoxy->SetType(dielectric_dielectric);
  OpticalEpoxy->SetFinish(ground);    // ground: follows Snell's law
  
  // For Cladding
  G4OpticalSurface *OpticalCladding = new G4OpticalSurface("CladdingSurface");
 OpticalCladding->SetModel(unified);
  OpticalCladding->SetType(dielectric_dielectric);
  OpticalCladding->SetFinish(ground);    // ground: follows Snell's law
  
  /*
  GROUND:The incidence of a photon upon a rough surface requires choosing the angle, a, 
  between a ‘micro-facet’ normal and that of the average surface. 
  The UNIFIED model assumes that the probability of micro-facet normals populates 
  the annulus of solid angle sin(a)dawill be  proportional to a gaussian of 
  SigmaAlpha:theOpSurface->SetSigmaAlpha(0.1);where sigma_alpha is in [rad]
  In the GLISUR model this is indicated by the value of polish; when it is <1, 
  then a random point is generated in a sphere of radius (1-polish), and 
  the corresponding vector is added to the normal. The value 0 means maximum 
  roughness with effective plane of reflection distributedas cos(a).theOpSurface -> SetPolish (0.0)
  */

  // For Fiber
  G4OpticalSurface *OpticalFiber = new G4OpticalSurface("FiberSurface");
  OpticalFiber->SetModel(unified);
  OpticalFiber->SetType(dielectric_dielectric);
  OpticalFiber->SetFinish(polished);    // polishdfronpainted: only reflection, absorption and no refraction
                                                               // polished: follows Snell's law
  //  OpticalFiber -> SetMaterialPropertiesTable(mptWLSfiber);
  
  // For Air
  G4OpticalSurface *OpticalAir = new G4OpticalSurface("AirSurface");
  OpticalAir->SetModel(unified);
  OpticalAir->SetType(dielectric_dielectric);
  OpticalAir->SetFinish(polished);    // polishdfronpainted: only reflection, absorption and no refraction

  /*
  //
  // Optical border surface
  //
  new G4LogicalBorderSurface("Panel/Air", PanelPV,  WorldPV, OpticalPanel);
  new G4LogicalBorderSurface("Air/Panel", WorldPV,  PanelPV, OpticalPanel);

  new G4LogicalBorderSurface("Panel/Epoxy", PanelPV,  EpoxyPV, OpticalPanel);
  new G4LogicalBorderSurface("Epoxy/Panel", EpoxyPV,  PanelPV, OpticalPanel);

  new G4LogicalBorderSurface("Cladding/Epoxy", CladdingPV,  EpoxyPV, OpticalPanel);
  new G4LogicalBorderSurface("Epoxy/Cladding", EpoxyPV,  CladdingPV, OpticalPanel);

  new G4LogicalBorderSurface("Cladding/Fiber", CladdingPV,  FiberPV, OpticalPanel);
  new G4LogicalBorderSurface("Fiber/Cladding", FiberPV,  CladdingPV, OpticalPanel);

  new G4LogicalBorderSurface("Air/Fiber",  WorldPV, FiberPV,  OpticalFiber);
  new G4LogicalBorderSurface("Fiber/Air",  FiberPV, WorldPV,  OpticalFiber);   

  */
  
  //
  // Define a simple mirror-like property for the wrapping material
  //
  G4double fWrappingPolish = 1;
  G4OpticalSurface* wrappingSurface = new G4OpticalSurface("WrappingSurface",
							   glisur,
							   ground,
							   dielectric_metal,
							   fWrappingPolish);
  
  G4MaterialPropertiesTable* wrappingSurfaceProperty =  new G4MaterialPropertiesTable();
  
  G4double p_wrapping[] = {2.00*eV, 3.47*eV};    // Since these properties do not depend on photon energies, simply take a two-element array
  const G4int nbins = sizeof(p_wrapping)/sizeof(G4double);
  G4double fWrappingReflectivity = 1.0;
  G4double refl_wrapping[] = {fWrappingReflectivity, fWrappingReflectivity};
  assert(sizeof(refl_wrapping) == sizeof(p_wrapping));
  G4double effi_wrapping[] = {0, 0};
  assert(sizeof(effi_wrapping) == sizeof(effi_wrapping));

  wrappingSurfaceProperty->
    AddProperty("REFLECTIVITY",p_wrapping, refl_wrapping,nbins);
  wrappingSurfaceProperty->
    AddProperty("EFFICIENCY",p_wrapping, effi_wrapping,nbins);
  
  wrappingSurface -> SetMaterialPropertiesTable(wrappingSurfaceProperty);

  // Use G4LogicalSkinSurface for one-directional photon propagation
  new G4LogicalSkinSurface("WrappingSurface", WrappingLV, wrappingSurface);
  
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

  // sensitive detectors -----------------------------------------------------
  auto sdManager = G4SDManager::GetSDMpointer();
  G4String SDname;
  
  auto sipmSD = new FPSiPMSD(SDname="/sipmSD");
  sdManager->AddNewDetector(sipmSD);
  sipmLV->SetSensitiveDetector(sipmSD);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
