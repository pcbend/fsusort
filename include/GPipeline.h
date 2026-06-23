#ifndef __GPIPELINE_H__
#define __GPIPELINE_H__

#include <string>
#include <vector>

class evtLoop;
class ddasLoop;

class GPipeline {
public:
  GPipeline();
  ~GPipeline();

  void AddFiles(const std::vector<std::string> &filename);
  
  void SetDetmap(const std::string& filename) { fDetmap = filename; }
  void SetOrderingWindow(uint64_t window)     { fOrderingWindow = window; } // evtLoop buffer
  void SetBuildWindow(double window)          { fBuildWindow = window;    } // ddasLoop event window
  void SetNPhysicsThreads(size_t n)           { fNPhysicsThreads = n;     }

  bool Run();              // blocking sort
  bool RunFile(const std::string& filename);

private:
  bool RunFileLinear(const std::string& filename);
  void PrintProgress(const evtLoop& reader, const ddasLoop& converter) const;

  std::vector<std::string> fFiles;
  std::string fDetmap{"cals/detmap.tsv"};

  uint64_t fOrderingWindow{500000}; // current physicsTest value
  double   fBuildWindow{200};       // current physicsTest value
  size_t   fNPhysicsThreads{1};
};




#endif
