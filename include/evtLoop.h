#ifndef __EVTLOOP_H__
#define __EVTLOOP_H__

#include <atomic>
#include <cstdint>
#include <string>
#include <utility>

#include <GThread.h>
#include <GBuffer.h>
#include <evtFile.h>
#include <dataBlock.h>

struct evtStats {
  uint64_t blocksRead{0};
  uint64_t skippedItems{0};
  int64_t  filePos{0};
  int64_t  fileSize{0};

  double Percent() const {
    if(fileSize <= 0) return 0.0;
    return 100.0 * static_cast<double>(filePos) / static_cast<double>(fileSize);
  }
};

class evtLoop : public GThread {
public:
  explicit evtLoop(const std::string& filename,
                   uint64_t orderingWindow = 0,
                   bool isNSCL = false)
    : fFile(filename, isNSCL),
      fBuffer(orderingWindow) {}

  dataBlockBuffer& Buffer() { return fBuffer; }

  bool Finished() const { return fFinished; }
  bool Empty() const { return fBuffer.Empty(); }

  bool TryPop(dataBlock& block) {
    return fBuffer.TryPop(block);
  }

  evtStats GetStats() const {
    evtStats s;
    s.blocksRead  = fBlocksRead;
    s.skippedItems = fSkippedItems;
    s.filePos     = fFile.GetFilePos();
    s.fileSize    = fFile.GetFileSize();
    return s;
  }

protected:
  void Iteration() override {
    dataBlock block;

    const int status = fFile.ReadBlock(block, 0);

    if(status == 1) {
      fBlocksRead++;
      fBuffer.Push(block.time, std::move(block));
      return;
    }

    if(status == -2) {
      fSkippedItems++;
      return; // skip NSCL non-physics item, keep reading
    }

    // EOF or error: release remaining ordered blocks, mark finished, stop loop
    Flush();
    RequestStop();
  }

  void Flush() override {
    fBuffer.Flush();   // tells buffer to stop holding back ordering-window tail
    fFinished = true;
  }

private:
  evtFile fFile;
  dataBlockBuffer fBuffer;
  std::atomic<uint64_t> fBlocksRead{0};
  std::atomic<uint64_t> fSkippedItems{0};



  std::atomic<bool> fFinished{false};
};

#endif
