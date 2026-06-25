
#include<LaBr.h>

LaBrHit::LaBrHit() { } 

LaBrHit::~LaBrHit() { } 

void LaBrHit::Clear() {
  address   = 0x0;
  id        = -1;
  eqdc      = -1;
  ecal      = -1;
  timestamp = -1;
  cfd       = -1;
  time      = -1;
}

bool LaBrHit::Build(const ddasHit &fragment) {
  address   = fragment.GetAddress();
  id        = fragment.GetId();
  if(fragment.hasQDC && !fragment.GetForcedCFD()) {
    double add = fragment.GetQDC()[2] + fragment.GetQDC()[3];
    double sub = fragment.GetQDC()[1];
    eqdc = add-sub*(140./100.);
  } else {
   eqdc = 0;
  }
  ecal      = fragment.GetEcal_labr();
  timestamp = fragment.GetTimestamp();
  cfd       = fragment.GetCFD();
  time      = fragment.GetTime();
  return true;
}

double LaBrHit::GetFastTime() const {
  //return 2ns timing.
  return (timestamp*5 + cfd)*2;
}






LaBr::LaBr() { } 

LaBr::~LaBr() { } 

void LaBr::Clear() {
  hits.clear();
  GDetector::Clear();
}



int LaBr::BuildHits() { 
  LaBrHit hit;
  //bool build = false;
  for(const auto& fragment : fRawHits) {
    hit.Build(fragment);
    hits.emplace_back(hit);
    hit.Clear();
  }
  return hits.size();
} 
