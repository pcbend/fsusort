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
//  double   GetCharge()   const { return charge; }  // raw energy filter output
//  double   GetEcal()     const { return ecal;   }  // calibrated energy filter output.
//  double   GetTime()     const { return time  +cfd; } 
//  double   GetCFDTime()  const { return cfd; } //conversion already handled
//  int      GetId()       const { return id;  } // cuurently set as crate*13*16 + (slot-2)*16 +ch 
//
//  const std::vector<unsigned short> &GetTrace() const { return trace; }
//  const std::array<int,8>           &GetQDC() const { return qdc; }
//  bool hasQDC{false};

// QDC INFO (Trinity)
// 0 - baseline far   31                  15 
// 1 - baseline near  29   (60)           30
// 2 - pre-peak       15                  15
// 3 - peak           20                  20
// 4 - mid            10   (45)           10
// 5 - tail           55   (100)          55 
// 6 - post-tail      15                  15
// 7 - post-post tail (basline) 25        40


// QDC INFO (Fast Timing) (timestamp units)
// 0 - baseline     100
// 1 - single       100
// 2 - tail          40
// 3 - fixed-window  40
// 4 - fixed-window  40
// 5 - fixed-window  40
// 6 - fixed-window  20 ?
// 7 - fixed-window  20 ?



// API for filling histograms in include/GHistogramer.h 

//fill histograms here. 
void MakeHistograms(const std::vector<ddasHit> &event) {
 
  double time1 =-1;
  double time2 =-1;

  for(auto hit : event) {
    
    if(hit.GetId()>=202 && hit.GetId() <=206) {
      if(time1<0) time1 = hit.GetTime();
      if(time1>0) time2 = hit.GetTime();
    }


    GHistogramer::Get().Fill("ecal",8000,0,4000,hit.GetEcal(),
                                       300,0,300,hit.GetId());
    GHistogramer::Get().Fill("raw",16000,0,16000,hit.GetCharge(),
                                       300,0,300,hit.GetId());
    
    GHistogramer::Get().Fill(Form("singles/det%03i",hit.GetId()),16000,0,16000,hit.GetCharge());
    
    
    if(hit.GetId()>=194) {
      if(hit.GetForcedCFD()) { 
    	GHistogramer::Get().Fill(Form("Forced_%i",hit.GetId()),10,0,10,2);
      } else {
    	GHistogramer::Get().Fill(Form("Forced_%i",hit.GetId()),10,0,10,4);
      }
    }

    if(hit.GetId()>=192 && hit.GetId()<=207) {  //fast timing
      if(hit.hasQDC && !hit.GetForcedCFD()) { 
        double add = hit.GetQDC()[2] + hit.GetQDC()[3]; 
        double sub = hit.GetQDC()[1];             
        GHistogramer::Get().Fill("QDCsummary",16000,0,0,add-sub*(140./100.) ,   // zero-zero are auto limits
                                              300,0,300,hit.GetId());
      }
    }
    
    if(hit.GetId()>=64 && hit.GetId()<=191) { //trinity 
      if(hit.hasQDC && !hit.GetForcedCFD()) { 
        double base  = hit.GetQDC()[0]+hit.GetQDC()[1];
        double peak  = hit.GetQDC()[3] - (base)*(20./45.);
        double tail  = hit.GetQDC()[5] - (base)*(55./45.);
        double total = (hit.GetQDC()[3]+hit.GetQDC()[4]+hit.GetQDC()[5]) - base*(85./45);

        GHistogramer::Get().Fill("QDCsummary",16000,0,0,total,   // zero-zero are auto limits
                                              300,0,300,hit.GetId());
        
        
        GHistogramer::Get().Fill("QDCRatio",16000,0,0,tail/peak,   // zero-zero are auto limits
                                            300,0,300,hit.GetId());
     
       
        GHistogramer::Get().Fill(Form("trinity/det%03i",hit.GetId()),1000,0,1,tail/peak,
                                                                     2000,0,64000,total);

      }
    }
  }

  if(time1>0 && time2>0 && time1!=time2) {
   GHistogramer::Get().Fill("dt",20000,-100,100,time1-time2);
  }	  

}


//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////


