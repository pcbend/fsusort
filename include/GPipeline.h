#ifndef __GPIPELINE_H__
#define __GPIPELINE_H__

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

class evtLoop;
class ddasLoop;

class GPipeline {
public:
  GPipeline();
  ~GPipeline();

  void AddFiles(const std::vector<std::string> &files) { fFiles = files; }
  
  void SetDetmap(const std::string& filename) { fDetmap = filename; }
  void SetOrderingWindow(uint64_t window)     { fOrderingWindow = window; } // evtLoop buffer
  void SetBuildWindow(double window)          { fBuildWindow = window;    } // ddasLoop event window
  void SetNPhysicsThreads(size_t n)           { fNPhysicsThreads = n;     }
  void SetMaxBufferedBlocks(size_t n)         { fMaxBufferedBlocks = n;   }
  void SetMaxBufferedEvents(size_t n)         { fMaxBufferedEvents = n;   }

  bool Run();              // blocking sort
  bool RunFile(const std::string& filename);

private:
  struct ProgressSnapshot {
    int64_t filePos{0};
    uint64_t blocks{0};
    uint64_t hits{0};
    uint64_t events{0};
    std::chrono::steady_clock::time_point started{std::chrono::steady_clock::now()};
    std::chrono::steady_clock::time_point time{std::chrono::steady_clock::now()};
  };

  bool RunFiles(const std::vector<std::string>& files);
  bool RunFileLinear(const std::string& filename);
  void PrintProgress(const evtLoop& reader, const ddasLoop& converter,
                     size_t finishedPhysics, size_t totalPhysics,
                     ProgressSnapshot& previous) const;

  std::vector<std::string> fFiles;
  std::string fDetmap{"cals/detmap.tsv"};

  uint64_t fOrderingWindow{500000}; // current physicsTest value
  double   fBuildWindow{200};       // current physicsTest value
  size_t   fNPhysicsThreads{1};
  size_t   fMaxBufferedBlocks{131072};
  size_t   fMaxBufferedEvents{4096};
};




#endif
