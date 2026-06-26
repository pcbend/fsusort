
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
  cfdForced = true;
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
  cfdForced = fragment.GetForcedCFD();
  return true;
}

double LaBrHit::GetFastTime() const {
  //return 2ns timing. -> BL: I think this is actually in ns, see below
  //cfd forced is evt*10ns (equation 4-8) and not forced is (evt*5 + correction)*2ns (equation 4-7). So if cfd correction is 0 then 4-7 and 4-8 should give same result and have same units. evt is in clock ticks (1/100 MHz), 1 tick is 10 ns so 4-7 and 4-8 multiply by 10 or 2*5 to convert ticks to units of ns.
  //if(cfdForced) return timestamp*10;
  if(cfd == -5) return timestamp*10;
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
