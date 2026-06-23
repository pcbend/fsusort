
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

  return RunFiles(fFiles);
}


bool GPipeline::RunFile(const std::string& filename) {
  return RunFiles({filename});
}


bool GPipeline::RunFiles(const std::vector<std::string>& files) {
  if(files.empty())
    return true;

  evtLoop reader(files, fOrderingWindow, false, fMaxBufferedBlocks);
  ddasLoop converter(reader, fBuildWindow, fNPhysicsThreads, fMaxBufferedEvents);

  std::vector<std::unique_ptr<physicsLoop>> physics;
  for(size_t i = 0; i < fNPhysicsThreads; ++i)
    physics.emplace_back(std::make_unique<physicsLoop>(converter, i));

  reader.Start();
  converter.Start();

  for(auto& p : physics)
    p->Start();

  ProgressSnapshot previous;
  auto nextProgress = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);

  while(true) {
    size_t finishedPhysics = 0;
    for(const auto& p : physics) {
      if(p->Finished())
        ++finishedPhysics;
    }
    const bool allFinished = finishedPhysics == physics.size();

    if(allFinished)
      break;

    const auto now = std::chrono::steady_clock::now();
    if(now >= nextProgress) {
      PrintProgress(reader, converter, finishedPhysics, physics.size(), previous);
      nextProgress = now + std::chrono::milliseconds(500);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }

  PrintProgress(reader, converter, physics.size(), physics.size(), previous);
  printf("\n\n");

  for(auto& p : physics)
    p->Stop();

  converter.Stop();
  reader.Stop();

  return true;
}


void GPipeline::PrintProgress(const evtLoop& reader,
                              const ddasLoop& converter,
                              size_t finishedPhysics, size_t totalPhysics,
                              ProgressSnapshot& previous) const {
  auto e = reader.GetStats();
  auto d = converter.GetStats();

  auto now = std::chrono::steady_clock::now();
  double dt = std::chrono::duration<double>(now - previous.time).count();

  double mbps = 0.0;
  double blockRate = 0.0;
  double hitRate = 0.0;
  double eventRate = 0.0;

  if(dt > 0.0) {
    mbps      = (e.filePos     - previous.filePos) / dt / 1024.0 / 1024.0;
    blockRate = (e.blocksRead  - previous.blocks)  / dt;
    hitRate   = (d.hitsBuilt   - previous.hits)    / dt;
    eventRate = (d.eventsBuilt - previous.events)  / dt;
  }

  const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - previous.started).count();
  const auto hours = elapsed / 3600;
  const auto minutes = (elapsed % 3600) / 60;
  const auto seconds = elapsed % 60;

  printf(CLEAR_LINE "[%02lld:%02lld:%02lld]  file %llu/%llu  %6.2f%%  %.1f / %.1f MB  %7.1f MB/s\n",
         (long long)hours,
         (long long)minutes,
         (long long)seconds,
         (unsigned long long)e.currentFile,
         (unsigned long long)e.totalFiles,
         e.Percent(),
         e.filePos / 1024.0 / 1024.0,
         e.fileSize / 1024.0 / 1024.0,
         mbps);

  printf(CLEAR_LINE "%s -> %s -> physics %zu/%zu   queues: blocks %llu / %llu   events %llu / %llu\n",
         reader.Finished() ? "reader done" : "reader",
         converter.Finished() ? "converter done" : "converter",
         finishedPhysics,
         totalPhysics,
         (unsigned long long)e.bufferedBlocks,
         (unsigned long long)e.maxBufferedBlocks,
         (unsigned long long)d.bufferedEvents,
         (unsigned long long)d.maxBufferedEvents);

  printf(CLEAR_LINE "blocks %llu (%7.0f/s) | hits %llu (%7.0f/s) | events %llu (%7.0f/s)",
         (unsigned long long)e.blocksRead,
         blockRate,
         (unsigned long long)d.hitsBuilt,
         hitRate,
         (unsigned long long)d.eventsBuilt,
         eventRate);

  fflush(stdout);
  printf("\033[2A");
  fflush(stdout);

  previous.filePos = e.filePos;
  previous.blocks  = e.blocksRead;
  previous.hits    = d.hitsBuilt;
  previous.events  = d.eventsBuilt;
  previous.time    = now;
}
