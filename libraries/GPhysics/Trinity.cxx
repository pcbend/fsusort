
#include<Trinity.h>

#include<ddasHit.h>

#include<GHistogramer.h>

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
  if(!fragment.GetForcedCFD())
    time      = fragment.GetTime();
  else 
    time = -1;
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
    if((hit.id%2)==0) {
      hit.id = (hit.id-80)/2;
      hits_A.emplace_back(hit);
    } else {
      hit.id = (hit.id-80)/2;
      hits_B.emplace_back(hit);
    }
    hit.Clear();
  }
  for(const auto& hitA : hits_A) {
    for(const auto& hitB : hits_B) {
      if(hitA.id == hitB.id) {
        // check energy
        GHistogramer::Get().Fill("trinity/pair_energy",2000,-20000,20000,hitA.total  - hitB.total,
                                                    200,0,200,hitA.id);
        // check time
        if((hitA.time>0) && (hitB.time>0)) {
          GHistogramer::Get().Fill("trinity/pair_time",2000,-200,200,hitA.timestamp - hitB.timestamp,
                                                   200,0,200,hitA.id);
          GHistogramer::Get().Fill("trinity/pair_time_cfd",2000,-200,200,hitA.time - hitB.time,
                                                     200,0,200,hitA.id);
        }
        // pick which to take - right now, just taking A.
        hits.emplace_back(hitA);
        break;  
      }
      continue;
    }
  }
  hits_A.clear();
  hits_B.clear();
  return hits.size();
} 
