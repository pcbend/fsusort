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
  qdc.fill(0);
  hasQDC = false;
  traceLength=0;
  trace.clear();

}

void ddasHit::print() const {
  printf("ddas hit @ %f\n", GetTime()+GetCFDTime()/16384.);
  printf("\tevId:    %llu\n",evId);
  printf("\tid:      %i\n",id);
  printf("\tenergy:  %.1f\n",energy);
  printf("\thasQDC: %i\n", hasQDC);
  if(hasQDC) {
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

  //GChannel::Get(data.address())->Print();
  // check the digitizer 
  // set correctly depending on digitizer 
  evId = data.eventID;
  id   = data.crate*MAX_BOARDS_PER_CRATE*MAX_CHANNELS_PER_BOARD + 
        (data.slot-BOARD_START)*MAX_CHANNELS_PER_BOARD         + 
         data.ch;
  energy  = data.energy;
  address = data.address();

  time = static_cast<double>(data.time);
  cfd  = 0.0;
 
  auto *chan = GChannel::Get(data.address());


  switch(chan->fMHz) {
    case 100:
      if(data.cfd_forced==0) {
        cfd = static_cast<double>(data.cfd & 0x7fff)/32768.0;
      }
      break;
    case 250:
       if(data.cfd_forced == 0) {
        const double frac = static_cast<double>(data.cfd & 0x3fff) / 16384.0;
        const double src  = data.cfd_source ? 0.5 : 0.0;
        cfd = frac + src;
      }
      break;
    case 500:
     {
        const int source = (data.cfd >> 13) & 0x7;
        const int frac   = data.cfd & 0x1fff;

        if(source >=0 && source <=4) {
          cfd = (static_cast<double>(source) +
                 static_cast<double>(frac) / 8192.0) / 5.0;
        }
      }
      break;
    default:
      break;
  }

  //cfd corrected time values should be time-cfd.

  //if(data.QDCsum[0]>1) {
  //  hasQDC = true;
  //  for( int i = 0; i < 8; i++) {
  //    qdc[i] = data.QDCsum[i];
  //  }
  //}
hasQDC = (data.headerLength == 12 || data.headerLength == 16);

if(hasQDC) {
  for(int i = 0; i < 8; ++i)
    qdc[i] = data.QDCsum[i];
}




  traceLength = data.trace_length;
  // std::vector<unsigned short> trace;
  if(traceLength > 0) {
    trace.assign(data.trace.begin(), data.trace.end());
  } else {
    trace.clear();
  }
}



//ddasHit ddasHit::operator=(ddasHit const& rhs) {
//  ddasHit hit;
//  rhs.Copy(hit);
//  return hit;
//}

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

