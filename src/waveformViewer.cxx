
#include <iostream>
#include <string>

#include <TApplication.h>

#include <evtLoop.h>
#include <ddasLoop.h>
#include <GChannel.h>
#include <GWaveViewer.h>

#include <TVirtualPad.h>
#include <TCanvas.h>

static GWaveViewer* gViewer = nullptr;

void NextWaveformOnClick() {
  if(!gViewer || !gPad)
    return;

  if(gPad->GetEvent() == kButton1Down)
    gViewer->Next();
}


int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: waveformViewer file.evt\n";
    return 1;
  }

  std::string filename = argv[1];

  TApplication app("waveformViewer", &argc, argv);

  std::string homedir = std::getenv("HOME");
  GChannel::ReadDetmap(Form("%s/Packages/FSUSort/cals/detmap.tsv",homedir.c_str()));

  evtLoop  reader(filename, 500000);
  ddasLoop converter(reader, 200, 1);

  reader.Start();
  converter.Start();

  GWaveViewer viewer(converter);

  viewer.Draw();

  std::string line;



  while(true) {
    printf("QDC:\n");
    //for(int i=0;i<8;i++) { 
    //  printf("\t[%i]:\t%i\n",i,viewer.GetCurrentHit().GetQDC()[i]); 
    //}

      printf("\tGetTimestamp() = %.04f\n",viewer.GetCurrentHit().GetTimestamp());
      printf("\tGetCFD()       = %.04f\n",viewer.GetCurrentHit().GetCFD());
      printf("\tGetTime()      = %.04f\n",viewer.GetCurrentHit().GetTime());


      if(viewer.GetCurrentHit().GetForcedCFD()) printf(RED);
      printf("forced CFD: %i\n",viewer.GetCurrentHit().GetForcedCFD());
      if(viewer.GetCurrentHit().GetForcedCFD()) printf(RESET_COLOR);
      printf("QDC1 - QDC0:  %i\n",viewer.GetCurrentHit().GetQDC()[1]-viewer.GetCurrentHit().GetQDC()[0]); 
      printf("\t[0]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[0],viewer.GetCurrentHit().GetQDC()[0]/1.); 
      printf("\t[1]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[1],viewer.GetCurrentHit().GetQDC()[1]/1.); 
      printf("\t[2]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[2],viewer.GetCurrentHit().GetQDC()[2]/1.); 
      printf("\t[3]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[3],viewer.GetCurrentHit().GetQDC()[3]/1.); 
      printf("\t[4]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[4],viewer.GetCurrentHit().GetQDC()[4]/1.); 
      printf("\t[5]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[5],viewer.GetCurrentHit().GetQDC()[5]/1.); 
      printf("\t[6]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[6],viewer.GetCurrentHit().GetQDC()[6]/1.); 
      printf("\t[7]:\t%i\t%.02f\n",viewer.GetCurrentHit().GetQDC()[7],viewer.GetCurrentHit().GetQDC()[7]/1.); 
  
    std::cout << "[ENTER] next waveform, q to quit: ";

    std::getline(std::cin, line);

    if(line == "q" || line == "Q")
      break;

    if(!viewer.Next()) {
      std::cout << "No more traces.\n";
      break;
    }
  }

  converter.Stop();
  reader.Stop();

  return 0;
}

