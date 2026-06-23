#ifndef __TRINITY_H__
#define __TRINITY_H__

#include<GDetector.h>

class Trinity : public GDetector {
  public:
    Trinity();
    virtual ~Trinity();

    int BuildHits() override;
  private:

    std::vector<ddasHit> fCores;
    std::vector<ddasHit> fBGOs;


  ClassDefOverride(Trinity,1)
};

#endif
