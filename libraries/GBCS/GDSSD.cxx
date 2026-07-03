
#include<GDSSD.h>

#include<cstdio>

void GDSSD::Reset() { 
  fX = -1;
  fY = -1;
  fTime = -1;
  fFront.clear();
  fBack.clear();
}

void GDSSD::Print() const { 
  printf("front mult: %lu\t back mult: %lu\n",fFront.size(),fBack.size());
  for(const auto &hit : fFront) {
    printf("\t%i: %.1f\t%.1f\n",hit.GetId(),hit.GetEcal(),hit.GetTime());
  }
  printf("---------------------\n");
  for(const auto &hit : fBack) {
    printf("\t%i: %.1f\t%.1f\n",hit.GetId(),hit.GetEcal(),hit.GetTime());
  }
  printf("---------------------\n");
  printf("---------------------\n");

}



