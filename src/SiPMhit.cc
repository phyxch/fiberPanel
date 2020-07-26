/// Date created: July 1, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Updated: July 22, 2020
///                 Hexc, Zachary and Nadia
///                 Added G4 system unit header
///                 
///
/// Implement SiPM hit class:
///    This class stores information of a photon hit from the wls fiber

#include "SiPMhit.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4Allocator<SiPMHit> SiPMHitAllocator;

SiPMHit::SiPMHit()
{
  //  eDep = 0.0;
  photonCounts = 0;
}

SiPMHit::~SiPMHit()
{
}

void SiPMHit::Print()
{
  G4cout<<"     Print:: Pos = "<< position <<G4endl;
  G4cout <<"    Print:: phton counts = " << photonCounts << G4endl;
}
