#ifndef __CLARION_H__
#define __CLARION_H__

#include<GDetector.h>

class ddasHit;

class ClarionHit {
  public: 
    ClarionHit() { }   
    ~ClarionHit() { } 

    bool Build(const ddasHit&);
    void Clear();

    double GetFastTime() const;

  //private:
    int16_t   id;
    uint32_t  address;
    double    ecal;
    double    time;
    double    timestamp;
    double    cfd;

  ClassDef(ClarionHit,1)
};



class Clarion : public GDetector {
  public:
    Clarion();
    virtual ~Clarion();

    void Clear();
    int  BuildHits() override; 
  //private:
    std::vector<ClarionHit> hits;

  ClassDefOverride(Clarion,1)
};

#endif
