/// Date created: July 1, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Implementation of the SiPM sensitive detector
///

#include "FPSiPMSD.hh"

#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4HCtable.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPSiPMSD::FPSiPMSD(G4String SDname)
  : G4VSensitiveDetector(SDname),  photonHitCollection(0)
{  
  G4cout << "Creating SD with name: " << SDname << G4endl;

  //  'collectionName' is a protected data member of base class G4VSensitiveDetector
  collectionName.insert("SiPMHitCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FPSiPMSD::~FPSiPMSD()
{ }

G4bool FPSiPMSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
  // step is guaranteed to be in SiPM volume : no need to check for volume
  G4TouchableHandle  touchable = step->GetPreStepPoint()->GetTouchableHandle();

  // May need to check if one gets an optical photon here, instead of checking on the edep
  G4double edep = step->GetTotalEnergyDeposit();
  
  return true;
}

void FPSiPMSD::Initialize(G4HCofThisEvent* HCE)
{
  // Create a collection
  // -- collectionName[0] is "SiHitCollection", as declared in constructor
  std::cout<<"create new hitcollection "<<GetName()<<" "<<collectionName[0]<<std::endl;
  photonHitCollection = new SiPMHitCollection(GetName(), collectionName[0]);

  // -------------------------------------------------------------------------
  // -- and attachment of this collection to the "Hits Collection of this Event":
  // ----------------------------------------------------------------------------
  // -- To insert the collection, we need to get an index for it. This index
  // -- is unique to the collection. It is provided by the GetCollectionID(...)
  // -- method (which calls what is needed in the kernel to get this index).
  static G4int HCID = -1;
  if (HCID<0) HCID = GetCollectionID(0); // <<-- this is to get an ID for collectionName[0]
  HCE->AddHitsCollection(HCID, photonHitCollection);
}

void FPSiPMSD::EndOfEvent(G4HCofThisEvent* HE)
{
  G4cout << "EndOfEvent method of SD `" << GetName() << "'called." << G4endl;
  photonHitCollection->PrintAllHits();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
