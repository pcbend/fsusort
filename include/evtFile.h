#ifndef EVTFILE_H
#define EVTFILE_H



//#include <cstdio> 
//#include <iostream>
//#include <stdlib.h>
//#include <string.h>

#include <string>
#include <fstream>
#include <cstdint>

#include <TObject.h>

class dataBlock;

#include <globals.h>

class evtFile{

  public:
    //dataBlock * data;

  private:
    //FILE * inFile;
    std::ifstream inFile;
    void*         gzFileHandle{nullptr};
    bool          isGzip{false};
    size_t        gzipBytesSincePositionUpdate{0};

    size_t    inFileSize{0};
    size_t    inFilePos{0};
    bool      endOfFile{false};  
    bool      isOpened{false};
    int64_t   blockID{-1};
    int64_t   nBlock{0};
    
    uint32_t  extraHeader[14];
    uint32_t  traceBlock[MAX_TRACE_LENGTH/2];
    
    int32_t   *pxidata{NULL};
    int64_t   nWords{0};
    
    bool      isNSCL{false};
    
    uint32_t  rib_size[1];
    uint32_t  readRingItemByte{0};

  ///============================================ Methods
  public:

    evtFile();
    evtFile(std::string inFileName, bool isNSCL=false);
    ~evtFile();
    
    void OpenFile(std::string inFileName, bool isNSCL);
    void CloseFile();    

    void UpdateFileSize();
    bool IsEndOfFile()         const { return endOfFile; }
    
    bool    IsOpen()           const {return isOpened;}         
    int64_t GetFilePos()       const {return inFilePos;}
    int64_t GetFileSize()      const {return inFileSize;}
    int64_t GetBlockID()       const {return blockID;}
    int64_t GetNumberOfBlock() const {return nBlock;}
    
    
    int ReadBlock(dataBlock &data,int opt = 0);  /// 0 = default, fill data
                                                 /// 1 = no fill data
                                                 /// 2 = fill data and print

    void SetNSCL(bool isNSCL_Evt)  { isNSCL = isNSCL_Evt; }

  private:
    bool ReadBytes(void* dest, size_t nBytes);
    bool SkipBytes(size_t nBytes);
    void AdvanceFilePosition(size_t nBytes);
    void UpdateFilePosition(bool force = false);

  ClassDef(evtFile,0)
};

#endif
