
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
auto lastTime = std::chrono::steady_clock::now();

while(!physics.Finished()) {
  auto s = reader.GetStats();

  auto now = std::chrono::steady_clock::now();
  double dt = std::chrono::duration<double>(now - lastTime).count();

  double mbps = 0.0;
  if(dt > 0.0)
    mbps = (s.filePos - lastPos) / dt / 1024.0 / 1024.0;

  printf("\rfile=%6.2f%%  %.1f/%.1f MB  %7.1f MB/s  blocks=%llu  skipped=%llu",
         s.Percent(),
         s.filePos  / 1024.0 / 1024.0,
         s.fileSize / 1024.0 / 1024.0,
         mbps,
         (unsigned long long)s.blocksRead,
         (unsigned long long)s.skippedItems);

  fflush(stdout);

  lastPos = s.filePos;
  lastTime = now;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

printf("\n");


  physics.Stop();
  converter.Stop();
  reader.Stop();

  return 0;
}

