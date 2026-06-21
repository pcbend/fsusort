

#ifndef __GWAVEVIEWER_H__
#define __GWAVEVIEWER_H__

#include <vector>

#include <TCanvas.h>
#include <TGraph.h>

#include <ddasLoop.h>
#include <ddasHit.h>

class GWaveViewer {
  public:
    explicit GWaveViewer(ddasLoop& input);
    virtual ~GWaveViewer() = default;

    void Draw();
    bool Next();

  private:
    bool GetNextTrace(ddasHit& hit);
    void DrawHit(const ddasHit& hit);

    ddasLoop& fInput;

    TCanvas* fCanvas{nullptr};
    TGraph*  fGraph{nullptr};

    ddasHit fCurrentHit;
    std::vector<ddasHit> fEvent;

  ClassDef(GWaveViewer,0);
};

#endif

