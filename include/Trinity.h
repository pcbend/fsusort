#ifndef __TRINITY_H__
#define __TRINITY_H__

#include<GDetector.h>

class Trinity : public GDetector {
  public:
    Trinity();
    virtual ~Trinity();

    int BuildHits() override;
  private:

  ClassDefOverride(Trinity,1)
};

#endif
