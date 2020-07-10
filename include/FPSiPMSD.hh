/// Date created: July 1, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Define sensitive SiPM detector

#ifndef FPSiPMSD_h
#define FPSiPMSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include "SiPMhit.hh"

class G4HCofThisEvent;      // "H(it) C(ollection) of This Event

/// Run action class

class FPSiPMSD : public G4VSensitiveDetector
{
public:
  FPSiPMSD(G4String SDname);
  virtual ~FPSiPMSD();

  /// Mandatory base class method: it must to be overloaded
  G4bool ProcessHits(G4Step *step, G4TouchableHistory *ROhist);

  void Initialize(G4HCofThisEvent* HCE);
  void EndOfEvent(G4HCofThisEvent* HCE);
  
private:
  SiPMHitCollection*       photonHitCollection;  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

