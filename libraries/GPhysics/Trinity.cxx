
#include<Trinity.h>

#include<ddasHit.h>

TrinityHit::TrinityHit() { } 

TrinityHit::~TrinityHit() { } 

bool TrinityHit::Build(const ddasHit &hit) { return true; }

void TrinityHit::Clear() { }



Trinity::Trinity() { } 

Trinity::~Trinity() { } 

void Trinity::Clear() {
  hits.clear();
  GDetector::Clear();
}

int Trinity::BuildHits() { return 0; } 
