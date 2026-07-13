
#include <GBCS.h>


void GBCS::Reset() {
  fPin1.Reset();
  fPin2.Reset();
  fPin3.Reset();

  fI2N.Reset();
  fI2S.Reset();
  fI2TAC.Reset();

  fLowGain.Reset();
  fHighGain.Reset();

  fSSSD.Reset();
}
