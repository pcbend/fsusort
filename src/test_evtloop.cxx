#include <iostream>
#include <evtLoop.h>
#include <dataBlock.h>

#include <ddasHit.h>
#include <GChannel.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: test_evtloop file.evt\n";
    return 1;
  }

  GChannel::ReadDetmap("cals/detmap.tsv");

  evtLoop loop(argv[1], 500000); // 5 seconds;
  loop.Start();

  dataBlock block;
  size_t count = 0;

  ddasHit hit;
  int64_t lastTime = -1; 
  while(!loop.Finished() || !loop.Empty()) {
    if(loop.TryPop(block)) {
      ++count;
      hit.set(block);

    printf("0x%08x:\t %llu \t %.01f \t %.01f\n",hit.GetAddress(),block.time,hit.GetTime(), hit.GetTime() - lastTime);

    lastTime = hit.GetTime();

      //if(count < 1000) {
      //  std::cout << count
      //            << " time = " << block.time
      //            << std::endl;
      //}

      block.Clear();
    }
  }

  loop.Stop();

  std::cout << "read " << count << " blocks\n";
  return 0;
}
