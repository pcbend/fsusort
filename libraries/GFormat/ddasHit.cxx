#include<ddasHit.h>

#include<cmath>
#include<limits>


#include<dataBlock.h>
#include<GChannel.h>
#include<globals.h>

ddasHit::ddasHit() { Clear(); }

ddasHit::~ddasHit() { } 


void ddasHit::Clear() {
  evId   = ULLONG_MAX;
  id     = INT_MAX;
  energy = sqrt(-1);
  //time   = ULLONG_MAX;
  time   = std::numeric_limits<double>::quiet_NaN();
  cfd    = INT_MAX;
  //for(int i=0;i<8;i++) qdc[i] = INT_MAX;
  qdc.clear();
  traceLength=0;
  trace.clear();

}

void ddasHit::print() const {
  printf("ddas hit @ %f\n", GetTime()+GetCFDTime()/16384.);
  printf("\tevId:    %llu\n",evId);
  printf("\tid:      %i\n",id);
  printf("\tenergy:  %.1f\n",energy);
  printf("\tqdcSize: %lu\n",qdc.size());
  if(qdc.size()>0) {
    printf("\t\t");
    for(size_t i=0;i<qdc.size();i++) 
      printf("[%lu]:%i ",i,qdc.at(i));
    printf("\n");
  }
  printf("\ttraceLength: %i\n",traceLength);
  if(traceLength) {
    printf("\t\t");
    for(int i=0;i<traceLength;i++) {
      if(i!=0 && (i%16==0))
        printf("\n\t\t");
      printf("%i  ",trace.at(i));
    }
    printf("\n");
  }
  printf("---------------------------------\n");
  fflush(stdout);
}

void ddasHit::Copy(ddasHit& lhs) const {

  lhs.evId = evId;
  lhs.id   = id;

  lhs.setEvId(evId);
  lhs.setId(id);
  lhs.setEnergy(energy);
  lhs.setTime(time);
  lhs.setCFD(cfd);
  lhs.setQDC(qdc);
  lhs.setTraceLength(traceLength);
  lhs.setTrace(trace);



}

void ddasHit::set(const dataBlock& data) { 

  GChannel::Get(data.address())->Print();

  // check the digitizer 
  // set correctly depending on digitizer 
  evId = data.eventID;
  id   = data.crate*MAX_BOARDS_PER_CRATE*MAX_CHANNELS_PER_BOARD + 
    (data.slot-BOARD_START)*MAX_CHANNELS_PER_BOARD         + 
    data.ch;
  energy = data.energy;

  double t;
  int    c;
  switch(GChannel::Get(data.address())->fMHz) {
    case 100:
      c = data.cfd&0x7fff;
      if(data.cfd_forced==0) {
        t = data.time*10 ; //time in ns.
        c = data.cfd;    // (now the true time in ns should (be e_t -cfd/16384)
        c = c/32768.;
      } else {
        t = data.time*10;
        c = 0;
      }
      break;
    case 250:
      c = data.cfd&0x3fff;
      if(data.cfd_forced==0) {
        t = data.time*8 - (data.cfd_source==true ? 1: 0)*4; //time in ns.
        c = data.cfd*4; // (now the true time in ns should (be e_t -cfd/16384)
        c = c/16384.;
      } else {
        t = data.time*8;
        c = 0; // (now the true time in ns should (be e_t -cfd/16384)
      }
      break;
    case 500:
      {
      int cfd_trigger = (data.cfd&0xe000) >> 29;
      c = data.cfd&0x0fff;
      if(cfd_trigger==7) { // forced
       t = data.time*2;    //??
       c=0;
      } else {
        cfd = (data.cfd/8192 + cfd_trigger-1)*2; //ns.
        t = data.time*2;    //??
      }
      }
      break;
    default:
      break;
  }
  time = t;
  cfd = c;

  //cfd corrected time values should be time-cfd.

  if(data.QDCsum[0]>1) {
    for( int i = 0; i < 8; i++) {
      qdc.push_back(data.QDCsum[i]);
    }
  }

  traceLength = data.trace_length;
  // std::vector<unsigned short> trace;
}



ddasHit ddasHit::operator=(ddasHit const& rhs) {
  ddasHit hit;
  rhs.Copy(hit);
  return hit;
}

bool ddasHit::operator==(ddasHit const& rhs) {
  if( evId==rhs.evId && id==rhs.id) 
    return true;
  return false;
}

bool ddasHit::operator<(ddasHit const & rhs) const {
  //double right = rhs.time + rhs.cfd/16384.;
  //double left  = time + cfd/16384.;
  //return left<right;
  if(GetId() == rhs.GetId()) {
    double right = rhs.time + rhs.cfd/16384.;
    double left  = time + cfd/16384.;
    return left<right;
  }
  return GetId() <= rhs.GetId();
}

