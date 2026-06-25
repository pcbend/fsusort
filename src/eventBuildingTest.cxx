#include <iostream>
#include <evtLoop.h>
#include <ddasLoop.h>

#include <dataBlock.h>
#include <ddasHit.h>
#include <GChannel.h>

#include <set>
#include <globals.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: test_evtloop file.evt\n";
    return 1;
  }

  GChannel::ReadDetmap("cals/detmap2.tsv");

  evtLoop  reader(argv[1], 500000,true); // 5 ms;
  ddasLoop converter(reader,200,1);   // 10ns -> 200 = 2us

  reader.Start();
  converter.Start();
  //                                       //builtEvent.                         
  // reader -(dataBlock) -> converter -> (std::vector<ddasHit>) -> 
  // dataBlock block;
  
  double firstTime = -1;
  std::vector<ddasHit> event;
  while(!converter.Finished() || !converter.Empty()) {
    if(converter.TryPop(event)) {
      if(event.empty()) {
        event.clear();
        continue;
      }


      //std::cout << "event size = " << event.size() << "\n";
      printf("event size: %lu\n",event.size());
      std::set<uint32_t> chanSeen;
      firstTime = event.begin()->GetTime();
      for(const auto &hit : event) {
          if(chanSeen.count(hit.GetAddress())) 
            printf( RED "\t0x%08x fired more than once!" RESET_COLOR  "\n",hit.GetAddress());
          chanSeen.insert(hit.GetAddress());

if(hit.GetAddress()==0x00000d05) printf(BLUE);

        //printf("\t0x%08x:\t %llu \t %.01f \t %.01f\n",hit.GetAddress(),block.time,hit.GetTime(), hit.GetTime() - lastTime);
        printf("\t0x%08x:\t %i  %.01f \t %.01f\n",hit.GetAddress(),hit.GetId(), hit.GetTime(), hit.GetTime() - firstTime);
if(hit.GetAddress()==0x00000d05) printf(RESET_COLOR);
      }
      printf("\n");
      event.clear();
    }
  }
  converter.Stop();
  reader.Stop();
  


  return 0;
}
