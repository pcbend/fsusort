#ifndef __GCHANNEL_H__
#define __GCHANNEL_H__


#include <string>
#include <vector>
#include <cstdint>
#include <map>

#include <TObject.h>

class GChannel { 
  private:
    GChannel();
  public: 
    ~GChannel();

    void Print() const; 

    static void ReadDetmap(std::string fname); 
    static GChannel *ParseLine(std::string line);
    static int  Size() { return fAddressMap.size(); }
    static GChannel* Get(uint32_t address) { return fAddressMap[address]; }


  //private:
    uint32_t fAddress;
    uint32_t fMHz;

    uint32_t fCrate;
    uint32_t fSlot;
    uint32_t fChannel;

    uint32_t fNumber; 

    std::string fName;
    std::vector<double> fCalPars;

    

    static std::map<uint32_t,GChannel*> fAddressMap; 
    static std::map<uint32_t,GChannel*> fNumberMap; 

  ClassDef(GChannel,0)
};

#endif
