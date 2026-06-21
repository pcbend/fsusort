
#include <GWaveViewer.h>

#include <iostream>

#include <TSystem.h>

GWaveViewer::GWaveViewer(ddasLoop& input)
  : fInput(input) {

  fCanvas = new TCanvas("cWaveform", "Waveform Viewer", 1000, 700);
  fGraph  = new TGraph();

  fCanvas->cd();
}

void GWaveViewer::Draw() {
  Next();
}

bool GWaveViewer::Next() {
  if(!GetNextTrace(fCurrentHit)) {
    std::cout << "No more traces found.\n";
    return false;
  }

  DrawHit(fCurrentHit);
  return true;
}

bool GWaveViewer::GetNextTrace(ddasHit& hit) {
  while(!fInput.Finished() || !fInput.Empty()) {
    if(!fInput.TryPop(fEvent))
      continue;

    for(const auto& candidate : fEvent) {
      if(!candidate.GetTrace().empty()) {
        hit = candidate;
        fEvent.clear();
        return true;
      }
    }

    fEvent.clear();
  }

  return false;
}

void GWaveViewer::DrawHit(const ddasHit& hit) {
  const auto& trace = hit.GetTrace();

  fCanvas->cd();
  fGraph->Set(0);

  for(int i = 0; i < static_cast<int>(trace.size()); ++i)
    fGraph->SetPoint(i, i, trace[i]);

  fGraph->SetTitle(Form("addr 0x%08x;sample;ADC", hit.GetAddress()));
  fGraph->Draw("AL");

  fCanvas->Modified();
  fCanvas->Update();

  gSystem->ProcessEvents();
}

