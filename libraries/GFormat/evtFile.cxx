
#include <evtFile.h>
#include <dataBlock.h>

#include <algorithm>
#include <filesystem>
#include <limits>
#include <system_error>

#include <zlib.h>

namespace {
bool HasGzipExtension(const std::string& filename) {
  constexpr const char* extension = ".gz";
  return filename.size() >= 3 &&
    filename.compare(filename.size() - 3, 3, extension) == 0;
}
}

evtFile::evtFile(){
  //data       = new dataBlock();
}

evtFile::~evtFile(){
  CloseFile();
  //if(data)           delete data;
  if(pxidata)        delete pxidata;
}

evtFile::evtFile(std::string inFileName, bool isNSCL){ 
  //data       = new dataBlock();
  OpenFile(inFileName, isNSCL);
}

void evtFile::OpenFile(std::string inFileName, bool isNSCL){
  CloseFile();

  isGzip = HasGzipExtension(inFileName);
  std::error_code error;
  const auto fileSize = std::filesystem::file_size(inFileName, error);
  if(!error)
    inFileSize = static_cast<size_t>(fileSize);

  if(isGzip) {
    gzFileHandle = gzopen(inFileName.c_str(), "rb");
    if(!gzFileHandle) {
      printf("Cannot read file : %s \n", inFileName.c_str());
      CloseFile();
      return;
    }
  } else {
    inFile.open(inFileName.c_str(),std::ios::binary | std::ios::ate);
  }
  //= fopen(inFileName.c_str(), "r");
  if(!isGzip && !inFile.is_open()){
    printf("Cannot read file : %s \n", inFileName.c_str());
    return;
  }else{
    if(!isGzip) {
      if(error)
        inFileSize = static_cast<size_t>(inFile.tellg());
      inFile.seekg(0,std::ios::beg);
    }
    inFilePos = 0;

    //data.Clear();
    this->isNSCL = isNSCL;

    rib_size[0] = 0;
    readRingItemByte = 0;

    isOpened = true;
  }
};

void evtFile::CloseFile(){
  //fclose(inFile);
  if(inFile.is_open())
    inFile.close();
  if(gzFileHandle) {
    gzclose(reinterpret_cast<gzFile>(gzFileHandle));
    gzFileHandle = nullptr;
  }
  isOpened = false;
  //data.Clear();

  inFileSize = 0;
  inFilePos  = 0;
  nBlock     = 0;    
  blockID    = -1;
  endOfFile  = false;
  isNSCL     = false;
  isGzip     = false;
};

void evtFile::UpdateFileSize(){
  if(isGzip || !inFile.is_open()) return;
  std::streampos current = inFile.tellg();
  inFile.seekg(0,std::ios::end);
  inFileSize = static_cast<size_t>(inFile.tellg());
  inFile.clear();
  inFile.seekg(current);
}

bool evtFile::ReadBytes(void* dest, size_t nBytes) {
  if(nBytes == 0)
    return true;

  if(isGzip) {
    if(!gzFileHandle)
      return false;

    auto* out = reinterpret_cast<unsigned char*>(dest);
    size_t totalRead = 0;

    while(totalRead < nBytes) {
      const unsigned int chunkSize =
        static_cast<unsigned int>(std::min<size_t>(
            nBytes - totalRead,
            static_cast<size_t>(std::numeric_limits<unsigned int>::max())));

      const int bytesRead =
        gzread(reinterpret_cast<gzFile>(gzFileHandle), out + totalRead, chunkSize);

      if(bytesRead <= 0) {
        UpdateFilePosition();
        return false;
      }

      totalRead += static_cast<size_t>(bytesRead);
    }

    UpdateFilePosition();
    return true;
  }

  if(!inFile.read(reinterpret_cast<char*>(dest), nBytes)) {
    UpdateFilePosition();
    return false;
  }

  UpdateFilePosition();
  return true;
}

bool evtFile::SkipBytes(size_t nBytes) {
  if(nBytes == 0)
    return true;

  if(isGzip) {
    if(!gzFileHandle)
      return false;

    const z_off_t skipped =
      gzseek(reinterpret_cast<gzFile>(gzFileHandle),
             static_cast<z_off_t>(nBytes),
             SEEK_CUR);

    UpdateFilePosition();
    return skipped >= 0;
  }

  if(!inFile.seekg(static_cast<std::streamoff>(nBytes), std::ios::cur)) {
    UpdateFilePosition();
    return false;
  }

  UpdateFilePosition();
  return true;
}

void evtFile::UpdateFilePosition() {
  if(isGzip) {
    if(!gzFileHandle)
      return;

    const z_off_t compressedOffset =
      gzoffset(reinterpret_cast<gzFile>(gzFileHandle));

    if(compressedOffset >= 0)
      inFilePos = static_cast<size_t>(compressedOffset);

    return;
  }

  if(!inFile.is_open())
    return;

  const std::streampos current = inFile.tellg();
  if(current != std::streampos(-1))
    inFilePos = static_cast<size_t>(current);
}

int evtFile::ReadBlock(dataBlock &data, int opt) {

  if(!isOpened)    return -1;
  if(endOfFile)   return -1;

  if(isNSCL && readRingItemByte == rib_size[0]) {
    unsigned int rih[2] = {0};

    do {
      if(!ReadBytes(rih, sizeof(rih))) {
        endOfFile = true;
        return -1;
      }

      if(rih[1] != 30) {
        if(!SkipBytes(rih[0] - 8)) {
          endOfFile = true;
          return -1;
        }
      }
    } while(rih[1] != 30);

    unsigned int ribh[5] = {0};

    if(!ReadBytes(ribh, sizeof(ribh))) {
      endOfFile = true;
      return -1;
    }

    if(!ReadBytes(rib_size, sizeof(rib_size))) {
      endOfFile = true;
      return -1;
    }

    readRingItemByte = 4;
  }

  if(isNSCL) {
    if(rib_size[0] > 48 && readRingItemByte < rib_size[0]) {
      constexpr size_t skipBytes = 14 * sizeof(uint32_t);

      if(!SkipBytes(skipBytes)) {
        endOfFile = true;
        return -1;
      }
      readRingItemByte += skipBytes;
    } else {
      return -2;
    }
  }

  unsigned int header[4];

  if(!ReadBytes(header, sizeof(header))) {
    endOfFile = true;
    return -1;
  }

  readRingItemByte += sizeof(header);
  blockID++;

  data.headerLength = (header[0] >> 12) & 0x1F;
  data.eventLength  = (header[0] >> 17) & 0x3FFF;
  data.trace_length = (header[3] >> 16) & 0x7FFF;

  if(opt == 0 || opt == 2) {
    data.eventID      = blockID;
    data.ch           =  header[0] & 0xF;
    data.slot         = (header[0] >> 4) & 0xF;
    data.crate        = (header[0] >> 8) & 0xF;
    data.pileup       =  header[0] >> 31;
    data.time         = ((uint64_t)(header[2] & 0xFFFF) << 32) + header[1];
    data.cfd_forced   =  header[2] >> 16 & 0x8000;
    data.cfd_source   =  header[2] >> 16 & 0x4000;
    data.cfd          =  header[2] >> 16 & 0xFFFF;
    data.energy       =  header[3] & 0xFFFF;
    data.trace_out_of_range = header[3] >> 31;

    data.ClearQDC();
    data.trace.clear();

    const uint32_t extraWords =
      data.headerLength > 4 ? data.headerLength - 4 : 0;

    if(extraWords > 0) {
      if(!ReadBytes(extraHeader, sizeof(uint32_t) * extraWords)) {
        endOfFile = true;
        return -1;
      }

      readRingItemByte += sizeof(uint32_t) * extraWords;

      if(data.headerLength == 8 || data.headerLength == 16) {
        data.trailing = extraHeader[0];
        data.leading  = extraHeader[1];
        data.gap      = extraHeader[2];
        data.baseline = extraHeader[3];
      }

      if(data.headerLength == 12 || data.headerLength == 16) {
        int startID = data.headerLength > 12 ? 4 : 0;

        for(int i = 0; i < 8; i++)
          data.QDCsum[i] = extraHeader[i + startID];
      }
    } else {
      data.trailing = 0;
      data.leading  = 0;
      data.gap      = 0;
      data.baseline = 0;
    }

    const uint32_t traceWords =
      data.eventLength > data.headerLength
      ? data.eventLength - data.headerLength
      : 0;

//    if(traceWords > 0) {
//      //skipping traces.
//      //inFile.seekg(sizeof(uint32_t) * traceWords, std::ios::cur);
//      //inFilePos += sizeof(uint32_t) * traceWords;
//      //readRingItemByte += sizeof(uint32_t) * traceWords;
//      inFile.read(reinterpret_cast<char*>(traceBlock),
//          sizeof(uint32_t) * traceWords);
// 
//      inFilePos += sizeof(uint32_t) * traceWords;
//      readRingItemByte += sizeof(uint32_t) * traceWords;
// 
//      data.trace.resize(data.trace_length);
//      for(uint32_t i = 0; i < traceWords; ++i) {
//        data.trace[2*i + 0] =  traceBlock[i]        & 0xffff;
//        data.trace[2*i + 1] = (traceBlock[i] >> 16) & 0xffff;
//      }
//    }
if(traceWords > 0) {
  if(!SkipBytes(sizeof(uint32_t) * traceWords)) {
    endOfFile = true;
    return -1;
  }
  readRingItemByte += sizeof(uint32_t) * traceWords;

  data.trace_length = 0;
  data.trace.clear();
}




  }

  if(opt == 1) {
    const uint32_t remainingWords =
      data.eventLength > 4 ? data.eventLength - 4 : 0;

    if(remainingWords > 0) {
      if(!SkipBytes(sizeof(uint32_t) * remainingWords)) {
        endOfFile = true;
        return -1;
      }
      readRingItemByte += sizeof(uint32_t) * remainingWords;
    }
  }

  if(opt == 2)
    data.Print();

  return 1;
}

/*
   int evtFile::ReadBlock(dataBlock &data,int opt){

   if( inFile.eof() ) return -1;
   if( endOfFile    ) return -1;

   if( isNSCL && readRingItemByte == rib_size[0]) {
//============= Ring item header, [0] = size, [1] = type
unsigned int rih[2]= {0}; 
do{
if(!inFile.read(reinterpret_cast<char*>(rih),sizeof(rih))) {
endOfFile = true;
return -1;
}
inFilePos = static_cast<size_t>(inFile.tellg());//
if( rih[1] != 30 ) {
inFile.seekg(rih[0]-8,std::ios::cur);      
inFilePos = static_cast<size_t>(inFile.tellg());
///printf("%ld---- skip %d bytes\n", inFilePos, rih[0]-8);
}
}while(rih[1] != 30);

//=========== Ring item body header, [0] = size, [1] timestamp-low, [2] timestamp-high, [3] source ID, [4] = barrier type
unsigned int ribh[5]={0}; 
if(!inFile.read(reinterpret_cast<char*>(ribh), sizeof(ribh))) {
endOfFile = true;
return -1;
}
inFilePos = static_cast<size_t>(inFile.tellg());
unsigned long long timestamp = ((unsigned long long)ribh[2] << 32) + ribh[1]; 
///printf("%ld - Ring Item Body Header| size : %d, timestamp : %llu \n", inFilePos, ribh[0], timestamp);

//================================== Ring item body
if(!inFile.read(reinterpret_cast<char*>(rib_size),sizeof(rib_size))) {  
endOfFile = true;
return -1;
}
inFilePos = static_cast<size_t>(inFile.tellg());
///printf("%ld - Size of Ring Item Body : %d \n", inFilePos, rib_size[0]);
readRingItemByte = 4; 
}

if( isNSCL){
if(rib_size[0] > 48 && readRingItemByte < rib_size[0] ){
inFile.seekg(14*4, std::ios::cur);
inFilePos = static_cast<size_t>(inFile.tellg()); 
readRingItemByte += 14 * 4;
}else{
return -2;
}
}

// start of pixie info - i think.
unsigned int header[4]; ///read 4 header, unsigned int = 4 byte = 32 bits. 
if(!inFile.read(reinterpret_cast<char*>(header),sizeof(header))) {
endOfFile = true;
return -1;
}
readRingItemByte += sizeof(header);
blockID ++;


if( opt == 0 || opt == 2){
/// see the Pixie-16 user manual, Table4-2
data.eventID      = blockID;
data.ch           =  header[0] & 0xF ;
data.slot         = (header[0] >> 4) & 0xF;
data.crate        = (header[0] >> 8) & 0xF;
data.headerLength = (header[0] >> 12) & 0x1F;
data.eventLength  = (header[0] >> 17) & 0x3FFF;
data.pileup       =  header[0] >> 31 ;
data.time         = ((uint64_t)(header[2] & 0xFFFF) << 32) + header[1];
data.cfd_forced   =  header[2] >> 16  & 0x8000;
data.cfd_source   =  header[2] >> 16  & 0x4000;
data.cfd          =  header[2] >> 16  & 0xFFFF; //0x3FFF; // 0x3FFF for 250MHz , 0x7FFF for 100MHz
data.energy       = (header[3] & 0xFFFF ); 
data.trace_length = (header[3] >> 16) & 0x7FFF;
data.trace_out_of_range =  header[3] >> 31;

data.ClearQDC();
data.ClearTrace();

///======== read QDCsum
if( data.headerLength >= 4 ){
  inFile.read(reinterpret_cast<char*>(extraHeader),sizeof(unsigned int)*(data.headerLength-4));
  readRingItemByte += (data.headerLength-4)*4;
  if( data.headerLength == 8 || data.headerLength == 16){
    data.trailing = extraHeader[0];
    data.leading  = extraHeader[1];
    data.gap      = extraHeader[2];
    data.baseline = extraHeader[3];
  }
  if( data.headerLength == 12 || data.headerLength == 16){
    for( int i = 0; i < 8; i++){
      int startID = 0;
      if( data.headerLength > 12) startID = 4; ///the 1st 4 words
      data.QDCsum[i] = extraHeader[i+startID];
    }
  }
}else{
  for( int i = 0 ; i < 8; i++){ data.QDCsum[i] = 0;}
  data.trailing = 0;
  data.leading  = 0;
  data.gap      = 0;
  data.baseline = 0;
}
///====== read trace
if( data.eventLength > data.headerLength ){
  //fread(traceBlock, sizeof(unsigned int) * ( data.trace_length / 2 ), 1, inFile);

  inFile.read(reinterpret_cast<char*>(traceBlock),sizeof(unsigned int) * ( data.trace_length / 2) );
  readRingItemByte += data.trace_length / 2 * 4;
  data.trace.resize(data.trace_length);
  for( int i = 0; i < data.trace_length/2 ; i++){
    data.trace[2*i+0] =  traceBlock[i] & 0xFFFF ;
    data.trace[2*i+1] = (traceBlock[i] >> 16 ) & 0xFFFF ;
  }

  ///make QDC by trace
  /**
    if( data.headerLength == 4 || data.headerLength == 8 ) {
    for( int i = 0; i < 8; i++){ data.QDCsum[i] = 0;}
    for( int i = 0; i < data.trace_length; i++){
    if(   0 <= i && i <  31 ) data.QDCsum[0] += data.trace[i];
    if(  31 <= i && i <  60 ) data.QDCsum[1] += data.trace[i];
    if(  60 <= i && i <  75 ) data.QDCsum[2] += data.trace[i];
    if(  75 <= i && i <  95 ) data.QDCsum[3] += data.trace[i];
    if(  95 <= i && i < 105 ) data.QDCsum[4] += data.trace[i];
    if( 105 <= i && i < 160 ) data.QDCsum[5] += data.trace[i];
    if( 160 <= i && i < 175 ) data.QDCsum[6] += data.trace[i];
    if( 175 <= i && i < 200 ) data.QDCsum[7] += data.trace[i];
    }
    }*/
  /*    }
        }

        if( opt == 1 ){

        data.headerLength = (header[0] >> 12) & 0x1F;
        data.eventLength  = (header[0] >> 17) & 0x3FFF;
        data.trace_length = (header[3] >> 16) & 0x7FFF;

        if( data.headerLength >= 4 ){
        inFile.read(reinterpret_cast<char*>(extraHeader),sizeof(unsigned int) * (data.headerLength-4));
        }
        if( data.eventLength > data.headerLength ){
        inFile.read(reinterpret_cast<char*>(traceBlock),sizeof(unsigned int) * (data.trace_length/2));
        }
        }

        inFilePos = static_cast<size_t>(inFile.tellg()); 

        if( opt == 2) data.Print();

        return 1; 
        }
   */
