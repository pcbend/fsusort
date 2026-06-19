#ifndef __CLARION_H__
#define __CLARION_H__

#include<GDetector.h>

class Clarion : public GDetector {
  public:
    Clarion();
    virtual ~Clarion();

    int BuildHits() override; 
  private:

  ClassDefOverride(Clarion,1)
};

#endif
