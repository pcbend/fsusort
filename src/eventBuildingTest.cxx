#include <iostream>
#include <evtLoop.h>
#include <ddasLoop.h>

#include <dataBlock.h>
#include <ddasHit.h>
#include <GChannel.h>
#include <GHistogramer.h>

#include <set>
#include <globals.h>

#include <GBCS.h>


void ProcessEvent(GBCS &bcs,const std::vector<ddasHit> &event);

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: simpleHists file.evt\n";
    return 1;
  }

  std::string homedir = std::getenv("HOME");
  GChannel::ReadDetmap(Form("%s/Packages/FSUSort/cals/detmap3.tsv",homedir.c_str()));

  std::filesystem::path p(argv[1]);
  std::string ofile = "hist_" + p.stem().string() + ".root";

  GHistogramer::Get().SetOutFile(ofile);

  evtLoop  reader(argv[1], 500000,true); // 5 ms;
  ddasLoop converter(reader,200,1);   // 10ns -> 200 = 2us

  reader.Start();
  converter.Start();

  int64_t lastPos = 0;
  uint64_t lastBlocks = 0;
  uint64_t lastHits = 0;
  uint64_t lastEvents = 0;

  auto lastTime = std::chrono::steady_clock::now();
  auto lastPrint = lastTime;

  std::cout << HIDE_CURSOR << std::flush;

  std::vector<ddasHit> event;
  GBCS bcs;
  while(!converter.Finished() || !converter.Empty()) {
    if(converter.TryPop(event)) {
      ProcessEvent(bcs,event);
      event.clear();
    }

    auto now = std::chrono::steady_clock::now();

    if(now - lastPrint > std::chrono::milliseconds(500)) {
      auto e = reader.GetStats();
      auto d = converter.GetStats();

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
      lastPrint = now;
    }
  }

  printf(CURSOR_DOWN "\n" SHOW_CURSOR);




  converter.Stop();
  reader.Stop();

  GHistogramer::Get().Close();

  return 0;
}


void ProcessEvent(GBCS &bcs,const std::vector<ddasHit> &event) {
  bcs.Reset();

  for(const auto &hit : event) {
    switch(hit.GetId()) {
      case 0 ... 39:     // front High Gain
        bcs.fHighGain.AddFrontHit(hit);
        break;
      case 40 ... 79:    // front Low  Gain
        bcs.fLowGain.AddFrontHit(hit);
        break;
      case 80 ... 119:   // back  High Gain
        bcs.fHighGain.AddBackHit(hit);
        break;
      case 120 ... 159:  // back  Low  Gain
        bcs.fLowGain.AddBackHit(hit);
        break;
      case 176:
        bcs.fI2N.Unpack(hit);
        break;
      case 177:
        bcs.fI2S.Unpack(hit);
        break;
      case 180:
        bcs.fI2TAC.Unpack(hit);
        break;
      case 181:
        bcs.fPin1.Unpack(hit);
      case 182:
        bcs.fPin2.Unpack(hit);
      case 183:
        bcs.fPin3.Unpack(hit);
    

      default:
        break;
    }
  }

  //printf("LOW");
  //bcs.fLowGain.Print();
  //printf("HIGH");
  //bcs.fHighGain.Print();

  if((bcs.fPin1.Time() > 10) && (bcs.fI2N.Time()>10)) { 
    GHistogramer::Get().Fill("pid_S",4000,0,0,bcs.TOFS(),
                                  4000,0,16000,bcs.dE());
    GHistogramer::Get().Fill("pid_N",4000,0,0,bcs.TOFN(),
                                  4000,0,16000,bcs.dE());
    //printf("TOF: %.1f - %.1f \t\t %.1f\n",bcs.fI2N.Time(),bcs.fPin1.Time(),bcs.TOF());
  }


  //bcs.fPin1.Print();

} 




