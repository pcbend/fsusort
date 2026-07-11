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


#include <TApplication.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TSystem.h>


void ProcessEvent(GBCS &bcs,const std::vector<ddasHit> &event);

void DrawDSSD(const GBCS &bcs);

int main(int argc, char** argv) {


  if(argc < 2) {
    std::cerr << "usage: simpleHists file.evt\n";
    return 1;
  }

  // reading multiple .evt files 
  std::vector<std::string> InputFiles;
  for(int i=1; i<argc; ++i) {
    InputFiles.push_back(argv[i]);
  }
  std::sort(InputFiles.begin(), InputFiles.end());



  //TApplication *app = new TApplication("app",0,0);

  std::string homedir = std::getenv("HOME");
  GChannel::ReadDetmap(Form("%s/Packages/FSUSort/cals/detmap3.tsv",homedir.c_str()));

//  std::filesystem::path p(argv[1]);
//  std::string ofile = "hist_" + p.stem().string() + ".root";

// creating combined output filename
  std::filesystem::path p(InputFiles.front());
  std::string stem = p.stem().string();
  
  int run = -1;
  
  if(std::sscanf(stem.c_str(), "run-%d-%*d", &run) != 1) {
    std::cerr << "Invalid EVT filename: " << stem << "\n";
    return 1;
  }
  
  std::string ofile = Form("hist%04d.root", run);
  GHistogramer::Get().SetOutFile(ofile);



// evtLoop  reader(argv[1], 500000,true); // 5 ms;
  evtLoop  reader(InputFiles, 500000, true);  // 5 ms;
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


// DDAS timestamp continuity check
// double firstPin1Time = -1;
// double lastPin1Time = -1;



  std::vector<ddasHit> event;
  GBCS bcs;
  while(!converter.Finished() || !converter.Empty()) {
    if(converter.TryPop(event)) {
      ProcessEvent(bcs,event);
      
    // timestamp check
    
//    if(bcs.fPin1.Time() > 0) {
//      if(firstPin1Time < 0)
//        firstPin1Time = bcs.fPin1.Time();
//        lastPin1Time  = bcs.fPin1.Time();
//    }
      event.clear();
    }


//    if(firstPin1Time >=0 && lastPin1Time >= 0){    
//      printf("\nPIN1 timestamps:\n");
//      printf("  first raw: %.0f\n", firstPin1Time);
//      printf("  last raw:  %.0f\n", lastPin1Time);
//      
//      printf("  first sec: %.6f\n", firstPin1Time / 1.e8);
//      printf("  last sec:  %.6f\n", lastPin1Time / 1.e8);
//      printf("  duration:  %.6f sec\n",
//         (lastPin1Time - firstPin1Time) / 1.e8);
//      }else{
//      printf("\nNo PIN1 hits were found.\n");
//      }



    auto now = std::chrono::steady_clock::now();

    if(now - lastPrint > std::chrono::milliseconds(500)) {
      auto e = reader.GetStats();
      auto d = converter.GetStats();

      constexpr int barWidth = 30;
      
      double percent = e.Percent();
      int filled = static_cast<int>((percent / 100.0) * barWidth);
      filled = std::clamp(filled, 0, barWidth);
      
      std::string progressBar(filled, '#');
      progressBar.append(barWidth - filled, '-');

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

      printf(
          CLEAR_LINE "[%s] %6.2f%%  file %llu/%llu  %.1f/%.1f MB  %7.1f MB/s\n",
          progressBar.c_str(),
          percent,
          static_cast<unsigned long long>(e.currentFile),
          static_cast<unsigned long long>(e.totalFiles),
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

  bool hasPin1 = false;

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
        hasPin1 = true;
        bcs.fPin1.Unpack(hit);
      case 182:
        bcs.fPin2.Unpack(hit);
      case 183:
        bcs.fPin3.Unpack(hit);
    

      default:
        break;
    }
  }

//    if(hasPin1 && bcs.Triggered()) printf(RED);
//    else printf(BLUE);
//    printf("LOW");
//    bcs.fLowGain.Print();
//    printf("HIGH");
//    bcs.fHighGain.Print();
//    printf(RESET_COLOR);
//    printf("\n\n");
//
//    if(hasPin1 && bcs.Triggered()) DrawDSSD(bcs);


// Position Plot 
bcs.fHighGain.Build();
bcs.fLowGain.Build();

// High Gain Position
  if(bcs.fHighGain.HasPosition()) {
    GHistogramer::Get().Fill("dssd/High_gain_position",1000,0,80,bcs.fHighGain.X(),
                                                       1000,0,80,bcs.fHighGain.Y());
  }

// Low Gain Position
  if(bcs.fLowGain.HasPosition()) {
    GHistogramer::Get().Fill("dssd/Low_gain_position",1000,0,80,bcs.fLowGain.X(),
                                                      1000,0,80,bcs.fLowGain.Y());
  }

// Net Position [ if else statement to avoid double counting]
  if(bcs.fHighGain.HasPosition()) {
    GHistogramer::Get().Fill("dssd/net_position",800,0,80, bcs.fHighGain.X(),
                                             800,0,80, bcs.fHighGain.Y());
  } else if (bcs.fLowGain.HasPosition()) {
    GHistogramer::Get().Fill("dssd/net_position",800,0,80, bcs.fLowGain.X(),
                                             800,0,80, bcs.fLowGain.Y());
  }



// Validity check between PIN1 and I2N
  if((bcs.fPin1.Time() > 10) && (bcs.fI2N.Time()>10)) { 
    GHistogramer::Get().Fill("pid_N",4000,0,0,bcs.TOFN(),
                                  4000,0,16000,bcs.dE());
    GHistogramer::Get().Fill("tof_N",3600,0,7200,bcs.fPin1.Time()/1.e8,
                                     4000,0,64000,bcs.TOFN());
  }

// Validity check between PIN1 and I2S
  if((bcs.fPin1.Time() > 10) && (bcs.fI2S.Time()>10)) { 
    GHistogramer::Get().Fill("pid_S",4000,0,0,bcs.TOFS(),
                                  4000,0,16000,bcs.dE());
    GHistogramer::Get().Fill("tof_S",3600,0,7200,bcs.fPin1.Time()/1.e8,
                                     4000,0,64000,bcs.TOFS());
  }


// Front and Back strip v/s Channel ID (calibrated)
  for(const auto& hit : event) {
    const int id = hit.GetId();
  
    if(id >= 0 && id <= 79) {
      GHistogramer::Get().Fill("dssd/front_energy_vs_channel",4000,0,16000, hit.GetEcal(),
          80, 0, 80, id);
    }else if(id >= 80 && id <= 159) {
      GHistogramer::Get().Fill("dssd/back_energy_vs_channel",4000,0,16000, hit.GetEcal(),
          80, 80, 160, id);
    }
  }


// Front v/s Back Energy
 
  // restricting to multiplicity = 1
   if(bcs.fHighGain.fFront.size() == 1 && bcs.fHighGain.fBack.size() == 1) {
     const auto& front = bcs.fHighGain.fFront.front();
     const auto& back  = bcs.fHighGain.fBack.front();

     GHistogramer::Get().Fill("dssd/high_front_energy_vs_back_energy_mult=1",4000, 0, 16000, front.GetEcal(),
         4000, 0, 16000, back.GetEcal());
   }

 
  // pairing every hit 
    // High-gain front/back pairs
    for(const auto& front : bcs.fHighGain.fFront) {
      for(const auto& back : bcs.fHighGain.fBack) {
        GHistogramer::Get().Fill("dssd/front_energy_vs_back_energy",4000, 0, 32000, front.GetEcal(),
            4000, 0, 32000, back.GetEcal());
      }
    }
    
    // Low-gain front/back pairs
    for(const auto& front : bcs.fLowGain.fFront) {
      for(const auto& back : bcs.fLowGain.fBack) {
        GHistogramer::Get().Fill("dssd/front_energy_vs_back_energy",4000, 0, 32000, front.GetEcal(),
            4000, 0, 32000, back.GetEcal());
      }
    }

} 


// void DrawDSSD(const GBCS &bcs) {
// 
//   //TApplication app("app",0,0);
// 
//   TCanvas *c1 = new TCanvas;
//   TH2D low("low","low",40,0,40,40,0,40);
//   TH2D high("high","high",40,0,40,40,0,40);
//   low.SetStats(0);
//   high.SetStats(0);
// 
//   for(const auto& front:bcs.fLowGain.fFront) {
//     for(const auto& back:bcs.fLowGain.fBack) {
//       int fs = front.GetId() - 40;
//       int bs = back.GetId() - 120;
//       low.SetBinContent(fs,bs,front.GetEcal());
//     }
//   }
//   for(const auto& front:bcs.fHighGain.fFront) {
//     for(const auto& back:bcs.fHighGain.fBack) {
//       int fs = front.GetId() - 0;
//       int bs = back.GetId() - 80;
//       high.SetBinContent(fs,bs,front.GetEcal());
//     }
//   }
//   c1->Divide(2,1);
//   c1->cd(1);
//   low.Draw("colz");
//   c1->cd(2);
//   high.Draw("colz");
// 
//   c1->Modified(); c1->Update();
// 
//   gSystem->ProcessEvents();
//   
//   std::cin.get();
// 
//   //app.Run(true);
// 
// }


