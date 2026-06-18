#ifndef __GTYPES_H__
#define __GTYPES_H__

#include<TObject.h>
#include<Rtypes.h>

class Gint;
R__EXTERN Gint *gInt;

enum class EConnectionType {
  kFILE,
  kTHREAD,
  kHIST,
  kVOID,
  kUNKNOWN
};

enum class EDetector {
  kUNKNOWN    = -1,
  kCLARION    = 10
};


enum class EFileType {
  kUNKNOWN          =  -1,
  kDAT              =   2, 
  kGEB              =   3,
  kMID              =   4,
  kEVT              =   5,
  kROOTFILE         = 256,
  kMACRO            = 257,
  kCALIBRATION      = 512,
  kCUTS             = 2052
};

EFileType DetermineFileType(const std::string& filename);

struct FileInfo {
  int run        = -1;
  int subrun     = -1;
  std::string matched; 
  
  EFileType type = EFileType::kUNKNOWN;
  const char * base{nullptr};
  size_t size{0};
};


struct Rec {
  uint64_t timestamp;
  uint32_t type;
  uint32_t size;
  uint64_t offset;
  uint64_t seq;
  bool operator<(const Rec& other) const { 
    if(timestamp != other.timestamp) 
      return timestamp>other.timestamp; 
    return seq<other.seq; 
  }
  int64_t operator-(const Rec& other) const {
    return static_cast<int64_t>(timestamp) - static_cast<int64_t>(other.timestamp);
  }
  void print(const FileInfo&) const;
};


#endif
