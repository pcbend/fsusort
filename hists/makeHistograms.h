{

  //if(event.clarion.hits.size())
  //printf(RED "event.clarion.hits.size() = %lu" RESET_COLOR  "\n",event.clarion.hits.size()); 

  for(const auto& hit : event.clarion.hits) {
    GHistogramer::Get().Fill("clarion/summary",4000, 0, 8000, hit.ecal,
                                               70, 0, 70, hit.id);
    for(const auto& hit2 : event.clarion.hits) {
      if(hit.id==hit2.id) continue;
      double e,dt1,dt2;
      if(hit.ecal > hit2.ecal) {
        e   = hit2.ecal;
        dt1 = hit.timestamp - hit2.timestamp;
        dt2 = hit.time - hit2.time;
      } else {
        e   = hit.ecal;
        dt1 = hit2.timestamp - hit.timestamp;
        dt2 = hit2.time - hit.time;
      }
      GHistogramer::Get().Fill("clarion/gg",4000,0,8000,hit.ecal,
                                             4000,0,8000,hit2.ecal);
      GHistogramer::Get().Fill("clarion/gg",4000,0,8000,hit2.ecal,
                                             4000,0,8000,hit.ecal);
      GHistogramer::Get().Fill("clarion/dtimestamp",2000,-1000,1000,dt1,
                                                     4000,0,4000,e);
      GHistogramer::Get().Fill("clarion/dtime",2000,-1000,1000,dt2,
                                                      4000,0,4000,e);
    }
  }

  for(const auto& hit : event.labr.hits) {
     GHistogramer::Get().Fill("labr/summary_ecal",4000,0,8000,hit.ecal,
                                                  190,209,20,hit.id);

    for(const auto& hit2 : event.labr.hits) {
      if(hit.id==hit2.id) continue;
      double e1,e2,t1,t2;
      e1 = hit.ecal;
      t1 = hit.time;
      e2 = hit2.ecal;
      t2 = hit2.time;

      GHistogramer::Get().Fill(Form("labr/dt_%i_%i",hit.id,hit2.id),5000,-10,10,t1-t2,2000,0,8000,e1);
      GHistogramer::Get().Fill(Form("labr/dt_%i_%i",hit2.id,hit.id),5000,-10,10,t2-t1,2000,0,8000,e2);
      GHistogramer::Get().Fill("labr/dt_sym_summary",5000,-10,10,t1-t2,2000,0,8000,e1);
      GHistogramer::Get().Fill("labr/dt_sym_summary",5000,-10,10,t2-t1,2000,0,8000,e2);

    }
  }
}
