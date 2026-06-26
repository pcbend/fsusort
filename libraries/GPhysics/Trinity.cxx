
#include<Trinity.h>

#include<ddasHit.h>

TrinityHit::TrinityHit() { } 

TrinityHit::~TrinityHit() { } 

bool TrinityHit::Build(const ddasHit &fragment) { 

  address   = fragment.GetAddress();
  id        = fragment.GetId();
  if(fragment.hasQDC && !fragment.GetForcedCFD()) {
    double base = fragment.GetQDC()[0] + fragment.GetQDC()[1];
    peak = fragment.GetQDC()[3] - base*(20./45.);
    tail = fragment.GetQDC()[5] - base*(55./45.);
    total = (fragment.GetQDC()[3] + fragment.GetQDC()[4] + fragment.GetQDC()[5]) - base*(85./45.);
  } else {
    peak = -1;
    tail = -1;
    total = -1;
  }
  ecal      = fragment.GetEcal();
  timestamp = fragment.GetTimestamp();
  cfd       = fragment.GetCFD();
  time      = fragment.GetTime();
  cfdForced = fragment.GetForcedCFD();
  return true;
 }

void TrinityHit::Clear() { 
  address   = 0x0;
  id        = -1;
  peak      = -1;
  tail      = -1;
  total     = -1;
  ecal      = -1;
  timestamp = -1;
  cfd       = -1;
  time      = -1;
  cfdForced = true;
}

double TrinityHit::GetFastTime() const {
  //Trinity is in 100 MHz module
  if(cfd == -5) return timestamp*10;
  return (timestamp + cfd)*10;
}

Trinity::Trinity() { } 

Trinity::~Trinity() { } 

void Trinity::Clear() {
  hits.clear();
  GDetector::Clear();
}

int Trinity::BuildHits() {
  TrinityHit hit;
  //bool build = false;
  for(const auto& fragment : fRawHits) {
    hit.Build(fragment);
    hits.emplace_back(hit);
    hit.Clear();
  }
  return hits.size();
} 
