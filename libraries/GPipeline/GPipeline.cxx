
#include <GPipeline.h>

#include <GChannel.h>
#include <evtLoop.h>
#include <ddasLoop.h>
#include <physicsLoop.h>

#include <chrono>
#include <memory>
#include <thread>
#include <vector>



GPipeline::GPipeline() { } 

GPipeline::~GPipeline() { } 

bool GPipeline::Run() {
  GChannel::ReadDetmap(fDetmap.c_str());

  for(const auto& file : fFiles) {
    if(!RunFile(file))
      return false;
  }

  return true;
}


bool GPipeline::RunFile(const std::string& filename) {
  evtLoop reader(filename, fOrderingWindow);
  ddasLoop converter(reader, fBuildWindow, fNPhysicsThreads);

  std::vector<std::unique_ptr<physicsLoop>> physics;
  for(size_t i = 0; i < fNPhysicsThreads; ++i)
    physics.emplace_back(std::make_unique<physicsLoop>(converter, i));

  reader.Start();
  converter.Start();

  for(auto& p : physics)
    p->Start();

  while(true) {
    bool allFinished = true;
    for(auto& p : physics)
      allFinished &= p->Finished();

    PrintProgress(reader, converter);

    if(allFinished)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  for(auto& p : physics)
    p->Stop();

  converter.Stop();
  reader.Stop();

  return true;
}


void GPipeline::PrintProgress(const evtLoop& reader,
                              const ddasLoop& converter) const {
  static int64_t lastPos = 0;
  static uint64_t lastBlocks = 0;
  static uint64_t lastHits = 0;
  static uint64_t lastEvents = 0;
  static auto lastTime = std::chrono::steady_clock::now();

  auto e = reader.GetStats();
  auto d = converter.GetStats();

  auto now = std::chrono::steady_clock::now();
  double dt = std::chrono::duration<double>(now - lastTime).count();

  double mbps = 0.0;
  double blockRate = 0.0;
  double hitRate = 0.0;
  double eventRate = 0.0;

  if(dt > 0.0) {
    mbps      = (e.filePos     - lastPos)    / dt / 1024.0 / 1024.0;
    blockRate = (e.blocksRead  - lastBlocks) / dt;
    hitRate   = (d.hitsBuilt   - lastHits)   / dt;
    eventRate = (d.eventsBuilt - lastEvents) / dt;
  }

  printf(CLEAR_LINE "file=%6.2f%% %.1f/%.1f MB %7.1f MB/s\n",
         e.Percent(),
         e.filePos / 1024.0 / 1024.0,
         e.fileSize / 1024.0 / 1024.0,
         mbps);

  printf(CLEAR_LINE "blocks=%llu (%7.0f/s) hits=%llu (%7.0f/s) events=%llu (%7.0f/s)",
         (unsigned long long)e.blocksRead,
         blockRate,
         (unsigned long long)d.hitsBuilt,
         hitRate,
         (unsigned long long)d.eventsBuilt,
         eventRate);

  fflush(stdout);
  printf(CURSOR_UP);
  fflush(stdout);

  lastPos    = e.filePos;
  lastBlocks = e.blocksRead;
  lastHits   = d.hitsBuilt;
  lastEvents = d.eventsBuilt;
  lastTime   = now;
}


