
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

// Defining front and back strip from channel IDs

static int FrontStrip(int id) {
  if(id >= 0 && id <= 39)
  return id;                  // returning Front High Gain 

  if(id >= 40 && id <=79)
  return id -40;              // returning Front Low Gain

  return -1;
}


static int BackStrip(int id) {
  if(id >= 80 && id <= 119)
  return id - 80;             // returning Back High gain

  if(id >= 120 && id <= 159)
  return id -120;             // returning Back Low gain

  return -1;
}

// Weighted average to get XY position == (front strip, back strip)

void GDSSD::Build() {
  fX = -1;
  fY = -1;

  if(fFront.empty() || fBack.empty())
  return;

  double frontWeightedSum = 0.0;
  double frontEnergySum = 0.0;

  for(const auto& hit : fFront) {
    const int strip = FrontStrip(hit.GetId());
    const double energy = hit.GetEcal();

    if(strip < 0 || energy <= 0)
    continue;

    frontWeightedSum += strip * energy;
    frontEnergySum += energy;
  }

  double backWeightedSum = 0.0;
  double backEnergySum = 0.0;

  for(const auto& hit : fBack) {
    const int strip = BackStrip(hit.GetId());
    const double energy = hit.GetEcal();

    if(strip < 0 || energy <= 0)
    continue;

    backWeightedSum += strip * energy;
    backEnergySum += energy;
  }

  if(frontEnergySum <= 0 || backEnergySum <= 0)
  return;

  fX = frontWeightedSum / frontEnergySum;
  fY = backWeightedSum / backEnergySum;
}


