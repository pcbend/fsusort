#ifndef __LaBr_H__
#define __LaBr_H__

#include<GDetector.h>

class LaBr : public GDetector {
  public:
    LaBr(); 
    virtual ~LaBr(); 

    int BuildHits() override; 
  private:

  ClassDefOverride(LaBr,1)
};

#endif
