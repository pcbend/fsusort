#ifndef __GDSSD_H__
#define __GDSSD_H__

#include <ddasHit.h>

#include<vector>


class GDSSD {
  public:
    GDSSD() = default;
    ~GDSSD() = default;

    void Reset();

    void AddFrontHit(const ddasHit &hit) { fFront.emplace_back(hit); }
    void AddBackHit(const ddasHit &hit)  { fBack.emplace_back(hit); }

    void Build();

    void Print() const;

  //private:
    double fX;
    double fY;
    
    double fTime;

    bool Triggered() const { return fFront.size() && fBack.size(); }

  //private:
    std::vector<ddasHit> fFront; //!
    std::vector<ddasHit> fBack;  //!

  ClassDef(GDSSD,0);
};

#endif
