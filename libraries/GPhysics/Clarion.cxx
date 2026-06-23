
#include<Clarion.h>

#include<globals.h>

bool ClarionHit::Build(const ddasHit& fragment) {
  id        = fragment.GetId();
  address   = fragment.GetAddress();
  ecal      = fragment.GetEcal();
  time      = fragment.GetTime();   
  timestamp = fragment.GetTimestamp();
  cfd       = fragment.GetCFD();
  return true;
}

void ClarionHit::Clear() {
  id        = -1;
  address   = 0x0;
  ecal      = -1;
  time      = -1;  
  timestamp = -1;
  cfd       = -1;
}



Clarion::Clarion() { } 

Clarion::~Clarion() { } 

void Clarion::Clear() {
  hits.clear();
  GDetector::Clear();

}

int Clarion::BuildHits() { 
  ClarionHit hit;
  bool build = false;
  for(const auto& fragment : fRawHits) {
    //printf("frag.id = %i\n",fragment.GetId());
    switch(fragment.GetId()) {
      case 0: //hpge poor resolution.
        break; 
      case 1 ... 3: //hpge 
        build = true;
        break;
      case 4: //bgo
        break;
      case 5 ... 9: //empty
        break;
      case 10 ... 13: //clover
        build = true;
        break;
      case 14: //bgo
        break;
      case 15: //empty
        break;
      case 16: //clover
        build = true;
        break;
      case 17: //hpge poor resolution
        break;
      case 18 ... 19: //clover
        build = true;
        break;
      case 20: //bgo
        break;
      case 21 ... 23: //clover
        build = true;
        break;
      case 24: //empty
        break;
      case 25: //bgo
        break;
      case 26 ... 32: //empty
        break;
      case 33 ... 34: //hpge
        build = true;
        break;
       case 35: // ??? bad   
        break;
      case 36: //bgo
        break;
      case 37 ... 38: //hpge
        build = true;
        break;
      case 39: // hpge bad
        break;
      case 40: //hpge
        build = true;
        break;
      case 41: //bgo
        break;
      case 42 ... 44: //hpge
        build = true;
        break;
      case 45: //hpge empty
        break;
      case 46: //bgo
        break;
      case 47: //hpge empty
        break;
      case 48 ... 51: //hpge
        build = true;
        break;
      case 52: //bgo
        break;
      default:
        break;
    }

    //printf("raw hit    = %lu\n", fRawHits.size());
    //printf(BLUE "built hits = %lu" RESET_COLOR "\n",hits.size());

    if(build) {
      hit.Build(fragment);
      hits.emplace_back(hit);
      build = false;
      hit.Clear();   
    }
  
  }
    // printf(BLUE "built hits = %lu" RESET_COLOR "\n",hits.size());
    //printf("raw hit    = %lu\n", fRawHits.size());
    //printf("built hits = %lu\n\n",hits.size());
  return hits.size();
} 
