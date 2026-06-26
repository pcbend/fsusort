#ifndef __TRINITY_H__
#define __TRINITY_H__

#include<GDetector.h>

class ddasHit;

class TrinityHit {
  public:
    TrinityHit();
    ~TrinityHit();

    bool Build(const ddasHit &hit);
    void Clear();

    double GetFastTime() const;

  //private:
    uint32_t address;
    int16_t  id;
    double peak;
    double tail;
    double total;
    double time;
    double timestamp;
    double cfd;
    double ecal;
    bool cfdForced{true};



  ClassDef(TrinityHit,1);
};



class Trinity : public GDetector {
  public:
    Trinity();
    virtual ~Trinity();

    int BuildHits() override;
    void Clear();
  //private:

    std::vector<TrinityHit> hits;


  ClassDefOverride(Trinity,1)
};

#endif
