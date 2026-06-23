#ifndef __LaBr_H__
#define __LaBr_H__

#include<GDetector.h>

#include <cstdint>

class ddasHit;

class LaBrHit {
  public:
    LaBrHit();
    ~LaBrHit();
   
    void Clear();
    bool Build(const ddasHit &hit);
 
  //private:
    uint32_t address;
    uint16_t  id;
    double   eqdc;
    double   ecal;
    double   timestamp;
    double   cfd;
    double   time;

  ClassDef(LaBrHit,1)
};

class LaBr : public GDetector {
  public:
    LaBr(); 
    virtual ~LaBr(); 

    void Clear();
    int BuildHits() override; 
  //private:
    std::vector<LaBrHit> hits;

  ClassDefOverride(LaBr,1)
};

#endif
