
#include <iostream>
#include <string>

#include <TApplication.h>

#include <evtLoop.h>
#include <ddasLoop.h>
#include <GChannel.h>
#include <GWaveViewer.h>

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

  app.Run();

  converter.Stop();
  reader.Stop();

  return 0;
}

