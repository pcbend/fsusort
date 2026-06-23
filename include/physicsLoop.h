
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

struct physicsEvent {
  Clarion  clarion;
  Trinity  trinity;
  LaBr     labr;

  void Clear();
  void Build(const std::vector<ddasHit>& hits);
};

class physicsLoop : public GThread {
public:
  physicsLoop(ddasLoop& input, std::size_t inputIndex = 0);

  bool Finished() const;

protected:
  void Iteration() override;
  void FillHistograms(const physicsEvent& event);
  void FillTree(const physicsEvent& event);
  void Flush() override;

private:
  ddasLoop& fInput;
  std::size_t fInputIndex{0};

  physicsEvent fEvent;

  std::atomic<bool> fFinished{false};
};

#endif

