
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
    ddasLoop(evtLoop& input, double buildWindow = 0.0,uint32_t nOutputs=1)
      : fInput(input) {
      for(uint32_t i=0;i<nOutputs;i++) 
        fBuffers.emplace_back(std::make_unique<ddasBuffer>(buildWindow));
    }

    ddasBuffer& Buffer(std::size_t i=0) {
      return *fBuffers.at(i);
    }

    bool Finished() const {
      return fFinished;
    }

    bool Empty(std::size_t i=0) const {
      return fBuffers.at(i)->Empty();
    }

    bool TryPop(std::vector<ddasHit>& event,std::size_t i=0) {
      return fBuffers.at(i)->TryPop(event);
    }

  protected:
    void Iteration() override {
      std::unique_ptr<dataBlock> block;

      if(fInput.TryPop(block)) {
        ddasHit hit;
        hit.set(*block);

        auto& buffer = *fBuffers[fNextOutput];
        buffer.Push(std::move(hit));
        fNextOutput = (fNextOutput+1)%fBuffers.size();
        //block->Clear();
        return;
      }

      if(fInput.Finished() && fInput.Empty()) {
        Flush();
        RequestStop();
        return;
      }
    }

    void Flush() override {
      for(auto& buffer : fBuffers)
        buffer->Flush();
      fFinished = true;
    }

  private:
    evtLoop& fInput;
    std::vector<std::unique_ptr<ddasBuffer> > fBuffers;
    std::size_t fNextOutput{0};
    std::atomic<bool> fFinished{false};
};

#endif



