#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include <cstdint>
#include <map>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

#include "dataBlock.h"
#include "ddasHit.h"

class dataBlockBuffer {
  public:
    explicit dataBlockBuffer(uint64_t orderingWindow = 0)
      : fOrderingWindow(orderingWindow) {}

    void Push(std::unique_ptr<dataBlock> block) {
      std::lock_guard<std::mutex> lock(fMutex);
      const uint64_t timestamp = block->time;
      if(timestamp > fLatestTimestamp)
        fLatestTimestamp = timestamp;
      fBuffer.emplace(timestamp, std::move(block));
    }

    bool TryPop(std::unique_ptr<dataBlock>& block) {
      std::lock_guard<std::mutex> lock(fMutex);

      if(fBuffer.empty())
        return false;
      if(!fFlushing && !HasReadyBlock())
        return false;

      auto it = fBuffer.begin();
      block = std::move(it->second);
      fBuffer.erase(it);
      return true;
    }

    void Flush() {
      std::lock_guard<std::mutex> lock(fMutex);
      fFlushing = true;
    }

    bool Empty() const {
      std::lock_guard<std::mutex> lock(fMutex);
      return fBuffer.empty();
    }

    std::size_t Size() const {
      std::lock_guard<std::mutex> lock(fMutex);
      return fBuffer.size();
    }

  private:
    bool HasReadyBlock() const {
      if(fBuffer.empty())
        return false;
      const auto oldestTimestamp = fBuffer.begin()->first;
      return oldestTimestamp + fOrderingWindow <= fLatestTimestamp;
    }
    mutable std::mutex fMutex;
    
    std::multimap<uint64_t, std::unique_ptr<dataBlock> > fBuffer;
    bool fFlushing{false};
    uint64_t fLatestTimestamp{0};
    uint64_t fOrderingWindow{0};
};

class ddasBuffer {
  public:
    explicit ddasBuffer(double buildWindow = 0.0)
      : fBuildWindow(buildWindow) { fCurrentEvent.reserve(256); }

    bool Push(ddasHit hit) {
      std::lock_guard<std::mutex> lock(fMutex);
      
      const double t = hit.GetTime();
      
      if(fCurrentEvent.empty()) {
        fCurrentStart = t;
        fCurrentEvent.emplace_back(std::move(hit));
        return false;
      }

      if(std::abs(t - fCurrentStart) <= fBuildWindow) { //GetTime is cfd corrected; abs for safety
        fCurrentEvent.emplace_back(std::move(hit));
        return false;
      }

      fBuiltEvents.push(std::move(fCurrentEvent));
      fCurrentEvent.clear();
      fCurrentEvent.emplace_back(std::move(hit));
      fCurrentStart = t;

      return true;
    }

    bool TryPop(std::vector<ddasHit>& event) {
      std::lock_guard<std::mutex> lock(fMutex);

      if(fBuiltEvents.empty())
        return false;
      event = std::move(fBuiltEvents.front());
      fBuiltEvents.pop();
      return true;
    }

    void Flush() {
      std::lock_guard<std::mutex> lock(fMutex);

      if(!fCurrentEvent.empty()) {
        fBuiltEvents.push(std::move(fCurrentEvent));
        fCurrentEvent.clear();
      }

      fFlushing = true;
    }

    bool Empty() const {
      std::lock_guard<std::mutex> lock(fMutex);
      return fBuiltEvents.empty() && fCurrentEvent.empty();
    }

    std::size_t Size() const {
      std::lock_guard<std::mutex> lock(fMutex);
      return fBuiltEvents.size();
    }

  private:
    double fBuildWindow{0.0};
    double fCurrentStart{0.0};

    std::vector<ddasHit> fCurrentEvent;
    std::queue<std::vector<ddasHit>> fBuiltEvents;

    mutable std::mutex fMutex;

    bool fFlushing{false};
};

#endif
