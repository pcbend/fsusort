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

protected:
  void Iteration() override {
    dataBlock block;

    const int status = fFile.ReadBlock(block, 0);

    if(status == 1) {
      fBuffer.Push(block.time, std::move(block));
      return;
    }

    if(status == -2) {
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

  std::atomic<bool> fFinished{false};
};

#endif
