
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

  while(!physics.Finished()) {
    // physicsLoop is doing the consuming/checking
    auto s = reader.GetStats();

    printf("\rfile=%6.2f%%  blocks=%llu  skipped=%llu",
        s.Percent(),
        (unsigned long long)s.blocksRead,
        (unsigned long long)s.skippedItems);

    fflush(stdout);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

  }

  physics.Stop();
  converter.Stop();
  reader.Stop();

  return 0;
}

