#ifndef __GDETECTOR_H__
#define __GDETECTOR_H__

#include <TObject.h>

#ifndef __CINT__
#include <ddasHit.h>
#else 
class ddasHit;
#endif


class GDetector {
  public: 
    GDetector()  = default;
    virtual ~GDetector() = default;

    virtual int BuildHits() =0;
    void InsertHit(const ddasHit &hit) { fRawHits.emplace_back(hit); }
    void Clear() { fRawHits.clear(); }

  //private:
    std::vector<ddasHit> fRawHits; //!

  ClassDef(GDetector,1)
};

#endif
