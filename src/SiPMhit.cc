/// Date created: July 1, 2020
/// Authors: hexc. Zachary Langford and Nadia Qutob
///
/// Implement SiPM hit class:
///    This class stores information of a photon hit from the wls fiber

#include "SiPMhit.hh"

// -- one more nasty trick for new and delete operator overloading:
G4Allocator<SiPMHit> SiPMHitAllocator;

SiPMHit::SiPMHit()
{
  eDep = 0.0;
}

SiPMHit::~SiPMHit()
{
}

void SiPMHit::Print()
{
  G4cout<<"     Pos = "<< position <<G4endl;
  G4cout <<"    eDep = " << eDep/MeV << G4endl;
}

