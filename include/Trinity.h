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
    uint16_t  id;     // after matching; i am going to use id
    double peak;     //   to store the new "trinity" number  
    double tail;     //   id = (id -80)/2; starts at zero.
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


    std::vector<TrinityHit> hits_A; //! even  temp container to match pairs.
    std::vector<TrinityHit> hits_B; //! odd   temp container to match pairs.


  ClassDefOverride(Trinity,1)
};

#endif
