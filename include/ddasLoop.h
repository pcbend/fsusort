
#ifndef __DDASLOOP_H__
#define __DDASLOOP_H__

#include <atomic>
#include <utility>

#include <GThread.h>
#include <GBuffer.h>
#include <evtLoop.h>
#include <dataBlock.h>
#include <ddasHit.h>

class ddasLoop : public GThread {
  public:
    ddasLoop(evtLoop& input, double buildWindow = 0.0)
      : fInput(input),
      fBuffer(buildWindow) {}

    ddasBuffer& Buffer() {
      return fBuffer;
    }

    bool Finished() const {
      return fFinished;
    }

    bool Empty() const {
      return fBuffer.Empty();
    }

    bool TryPop(std::vector<ddasHit>& event) {
      return fBuffer.TryPop(event);
    }

  protected:
    void Iteration() override {
      dataBlock block;

      if(fInput.TryPop(block)) {
        ddasHit hit;
        hit.set(block);

        fBuffer.Push(std::move(hit));
        block.Clear();
        return;
      }

      if(fInput.Finished() && fInput.Empty()) {
        Flush();
        RequestStop();
        return;
      }
    }

    void Flush() override {
      fBuffer.Flush();
      fFinished = true;
    }

  private:
    evtLoop& fInput;
    ddasBuffer fBuffer;

    std::atomic<bool> fFinished{false};
};

#endif



