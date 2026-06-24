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

  //private:


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
