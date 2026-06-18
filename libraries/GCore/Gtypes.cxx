
#include <Gtypes.h>
//#include <GBanks.h>
//empty file for cmake....


EFileType DetermineFileType(const std::string& filename) {
  size_t dot = filename.find_last_of('.');
  std::string ext = filename.substr(dot+1);

  if((ext=="gz") || (ext=="bz2") || (ext=="zip")) {
    std::string remaining = filename.substr(0,dot);
    ext = remaining.substr(remaining.find_last_of('.')+1);
  }
  
  if(ext == "dat") {
    return EFileType::kGEB;
  } else if(ext == "evt") {
    return EFileType::kEVT;
  } else if(ext == "mid") {
    return EFileType::kMID;
  } else if(ext == "cal") {
    return EFileType::kCALIBRATION;
  } else if(ext == "root") {
    return EFileType::kROOTFILE;
  } else if((ext=="c") || (ext=="C") 
            || (ext=="c+") || (ext=="C+") 
            || (ext=="c++") || (ext=="C++")) {
    return EFileType::kMACRO;
  } else if(ext == "cuts") {
    return EFileType::kCUTS;
  } else {
    return EFileType::kUNKNOWN;
  }
};



void Rec::print(const FileInfo &info) const {
  /*
  const char *recStart = info.base + offset;
  auto* header = reinterpret_cast<const GEBHeader*>(recStart);
  const char* payload = recStart + sizeof(GEBHeader);
  std::cout << *header << std::endl;
  int counter = 0;
  while(counter<header->size) {
    printf("0x%04x ",*((unsigned short*)(payload+counter)));
    counter+=2;
    if((counter%32)==0)
      printf("\n");
  }
  printf("\n");
  */
}



