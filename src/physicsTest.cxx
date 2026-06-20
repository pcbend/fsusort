
#include <iostream>

#include <evtLoop.h>
#include <ddasLoop.h>
#include <physicsLoop.h>

#include <GChannel.h>
#include <globals.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: physicsLoopTest file.evt\n";
    return 1;
  }

  GChannel::ReadDetmap("cals/detmap.tsv");

  evtLoop     reader(argv[1], 500000); // 5 ms
  ddasLoop    converter(reader, 200, 1);     // 200 ticks = 2 us
  physicsLoop physics(converter, 0);

  reader.Start();
  converter.Start();
  physics.Start();

  int64_t lastPos = 0;
  uint64_t lastBlocks = 0;
  uint64_t lastHits = 0;
  uint64_t lastEvents = 0;

  auto lastTime = std::chrono::steady_clock::now();

  std::cout << HIDE_CURSOR << std::flush;

  while(!physics.Finished()) {
    auto e = reader.GetStats();
    auto d = converter.GetStats();

    auto now = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(now - lastTime).count();

    double mbps = 0.0;
    double blockRate = 0.0;
    double hitRate = 0.0;
    double eventRate = 0.0;

    if(dt > 0.0) {
      mbps      = (e.filePos - lastPos) / dt / 1024.0 / 1024.0;
      blockRate = (e.blocksRead - lastBlocks) / dt;
      hitRate   = (d.hitsBuilt - lastHits) / dt;
      eventRate = (d.eventsBuilt - lastEvents) / dt;
    }

    printf(CLEAR_LINE "file=%6.2f%%  %.1f/%.1f MB  %7.1f MB/s\n",
        e.Percent(),
        e.filePos  / 1024.0 / 1024.0,
        e.fileSize / 1024.0 / 1024.0,
        mbps);

    printf(CLEAR_LINE "blocks=%llu (%7.0f/s)  hits=%llu (%7.0f/s)  events=%llu (%7.0f/s)",
        (unsigned long long)e.blocksRead,
        blockRate,
        (unsigned long long)d.hitsBuilt,
        hitRate,
        (unsigned long long)d.eventsBuilt,
        eventRate);

    fflush(stdout);

    printf(CURSOR_UP);
    fflush(stdout);

    lastPos = e.filePos;
    lastBlocks = e.blocksRead;
    lastHits = d.hitsBuilt;
    lastEvents = d.eventsBuilt;
    lastTime = now;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  printf(CURSOR_DOWN "\n" SHOW_CURSOR);


  physics.Stop();
  converter.Stop();
  reader.Stop();

  return 0;
}

