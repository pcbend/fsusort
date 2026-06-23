#include <physicsLoop.h>

#include <GHistogramer.h>

void physicsEvent::Clear() {
  clarion.Clear();
  trinity.Clear();
  labr.Clear();
}

void physicsEvent::Build(const std::vector<ddasHit>& hits) {
  Clear();

  #include<hists/makeRawHistograms.h> 

  for(const auto& hit : hits) {
    //clarion.Add(hit);
    //trinity.Add(hit);
    //labr.Add(hit);
    switch(hit.GetId()) {
      case 0 ... 63:    //clarion
        clarion.InsertHit(hit);
        break;
      case 64 ... 191:  //trinity 
        trinity.InsertHit(hit); 
        break;
      case 192 ... 207:  //fast timing
        labr.InsertHit(hit);
        break;
      default:
        break;
    }
  }

  clarion.BuildHits();
  trinity.BuildHits();
  labr.BuildHits();
}

physicsLoop::physicsLoop(ddasLoop& input, std::size_t inputIndex)
  : fInput(input),
    fInputIndex(inputIndex) {}

bool physicsLoop::Finished() const {
  return fFinished;
}

void physicsLoop::Iteration() {
  std::vector<ddasHit> event;

  if(fInput.Buffer(fInputIndex).WaitPop(event)) {
    fEvent.Build(event);
    FillHistograms(fEvent);
    FillTree(fEvent);
    return;
  }

  Flush();
  RequestStop();
}

void physicsLoop::FillHistograms(const physicsEvent& event) {
  //later
  #include <hists/makeHistograms.h>
}

void physicsLoop::FillTree(const physicsEvent& event) {
  // later
}

void physicsLoop::Flush() {
  fFinished = true;
}
