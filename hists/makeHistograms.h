{

  for(const auto& hit : event.clarion.fRawHits) {
    GHistogramer::Get().Fill("clarion/summary",4000, 0, 8000, hit.GetEcal(),
                                               70, 0, 70, hit.GetId());
  }



}

