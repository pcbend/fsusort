#include <iostream>
#include <evtLoop.h>
#include <ddasLoop.h>

#include <chrono>
#include <thread>
#include <filesystem>
#include <cstdlib>

#include <dataBlock.h>
#include <ddasHit.h>
#include <GChannel.h>
#include <GHistogramer.h>

#include <globals.h>

void MakeHistograms(const std::vector<ddasHit> &event);

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: simpleHists file.evt\n";
    return 1;
  }

  std::string homedir = std::getenv("HOME");
  GChannel::ReadDetmap(Form("%s/Packages/FSUSort/cals/detmap.tsv",homedir.c_str()));

  std::filesystem::path p(argv[1]);
  std::string ofile = "hist_" + p.stem().string() + ".root";

  GHistogramer::Get().SetOutFile(ofile);

  evtLoop  reader(argv[1], 500000); // 5 ms;
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
  while(!converter.Finished() || !converter.Empty()) {
    if(converter.TryPop(event)) {
      MakeHistograms(event);
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

//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////


//Calibrations are not yet applied but will be sotred in:
//   "cals/detmap.tsv"
//   A channel is unique to a wire, the info in a chanel 
//   can be accessed via::  
//      GChannel *channel = GChannel::Get(hit.GetAddress());
//   Address are derived from crate/slot/channel number as it 
//   appears in the raw data. 
//
//See include/ddasHit.h for the full API. Truncated info below:
//ddasHit (useful data members)
//
//  uint32_t GetAddress()  const { return address; }
//  double   GetEnergy()   const { return energy; }
//  double   GetTime()     const { return time  +cfd; } 
//  double   GetCFDTime()  const { return cfd; } //conversion already handled
//  int      GetId()       const { return id;  } // cuurently set as crate*13*16 + (slot-2)*16 +ch 
//
//  const std::vector<unsigned short> &GetTrace() const { return trace; }
//  const std::array<int,8>           &GetQDC() const { return qdc; }
//  bool hasQDC{false};

// QDC INFO
// 0 - baseline far   31   
// 1 - baseline near  29   (60)
// 2 - pre-peak       15
// 3 - peak           20
// 4 - mid            10   (45)
// 5 - tail           55   (100)
// 6 - post-tail      15 
// 7 - post-post tail (basline) 25 

// API for filling histograms in include/GHistogramer.h 

//fill histograms here. 
void MakeHistograms(const std::vector<ddasHit> &event) {
  for(auto hit : event) {
    GHistogramer::Get().Fill("summary",16000,0,16000,hit.GetEnergy(),
                                       300,0,300,hit.GetId());
    if(hit.hasQDC) { 
      double add = hit.GetQDC()[2] + 
                   hit.GetQDC()[3] + 
                   hit.GetQDC()[4] + 
                   hit.GetQDC()[5];
      double sub = hit.GetQDC()[0] +             
                   hit.GetQDC()[1];             
      GHistogramer::Get().Fill("QDCsummary",16000,0,0,add-sub,   // zero-zero are auto limits
                                          300,0,300,hit.GetId());
    }
  }
}


//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////




