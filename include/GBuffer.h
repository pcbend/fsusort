#ifndef __GBUFFER_H_
#define __GBUFFER_H_


#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include <utility>

#include "dataBlock.h"

class dataBlockBuffer {
  public:
    explicit dataBlockBuffer(uint64_t orderingWindow = 0) : fOrderingWindow(orderingWindow) {}

    void Push(uint64_t timestamp, dataBlock block) {
      {
        std::lock_guard<std::mutex> lock(fMutex);
        if(timestamp > fLatestTimestamp)
          fLatestTimestamp = timestamp;
        fBuffer.emplace(timestamp, std::move(block));
      }
      fCondition.notify_one();
    }

bool TryPop(dataBlock& block) {
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


    bool Empty() const {
      std::lock_guard<std::mutex> lock(fMutex);
      return fBuffer.empty();
    }

    std::size_t Size() const {
      std::lock_guard<std::mutex> lock(fMutex);
      return fBuffer.size();
    }

    void Flush() {
      std::lock_guard<std::mutex> lock(fMutex);
      fFlushing = true;
    }

  private:
    bool HasReadyBlock() const {
      if(fBuffer.empty())
        return false;
      const auto oldestTimestamp = fBuffer.begin()->first;
      return oldestTimestamp + fOrderingWindow <= fLatestTimestamp;
    }

    std::multimap<uint64_t, dataBlock> fBuffer;

    mutable std::mutex fMutex;
    std::condition_variable fCondition;

    bool fFlushing{false};

    uint64_t fLatestTimestamp{0};
    uint64_t fOrderingWindow{0};
};

#endif

