
#ifndef __PHYSICSLOOP_H__
#define __PHYSICSLOOP_H__

#include <atomic>
#include <cstddef>
#include <vector>

#include <GThread.h>
#include <ddasLoop.h>
#include <ddasHit.h>

#include <Clarion.h>
#include <Trinity.h>
#include <LaBr.h>

#include <GHistogramer.h>

struct physicsEvent {
  Clarion  clarion;
  Trinity  trinity;
  LaBr     labr;

  void Clear() {
    clarion.Clear();
    trinity.Clear();
    labr.Clear();
  }

  void Build(const std::vector<ddasHit>& hits) {
    Clear();

    for(const auto& hit : hits) {
      //clarion.Add(hit);
      //trinity.Add(hit);
      //labr.Add(hit);
      switch(hit.GetId()) {
        case 0 ... 64:
          clarion.InsertHit(hit);
          break;
        default:
          break;
      }
    }

    clarion.BuildHits();
    trinity.BuildHits();
    labr.BuildHits();
  }
};

class physicsLoop : public GThread {
public:
  physicsLoop(ddasLoop& input, std::size_t inputIndex = 0)
    : fInput(input),
      fInputIndex(inputIndex) {}

  bool Finished() const {
    return fFinished;
  }

protected:
  void Iteration() override {
    std::vector<ddasHit> event;

    if(fInput.TryPop(event, fInputIndex)) {
      fEvent.Build(event);
      FillHistograms(fEvent);
      FillTree(fEvent);
      return;
    }

    if(fInput.Finished() && fInput.Empty(fInputIndex)) {
      Flush();
      RequestStop();
    }
  }

  void FillHistograms(const physicsEvent& event) {
    //later 
    for(const auto &hit : event.clarion.fRawHits) {
      GHistogramer::Get().Fill("clarion/summary",4000,0,4000,hit.GetCharge(),
                                            70,0,70,hit.GetId());
    }                             


  }

  void FillTree(const physicsEvent& event) {
    // later
  }

  void Flush() override {
    fFinished = true;
  }

private:
  ddasLoop& fInput;
  std::size_t fInputIndex{0};

  physicsEvent fEvent;

  std::atomic<bool> fFinished{false};
};

#endif


