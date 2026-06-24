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

  double t1,e1,t2,e2;
  bool found1 =false;
  bool found2 =false;
  for(const auto& hit : event.labr.hits) {
    GHistogramer::Get().Fill("labr/summary_eqdc",8000, 0, 80000, hit.eqdc,
                                            30, 180, 210, hit.id);
    GHistogramer::Get().Fill("labr/summary_ecal",4000, 0, 8000, hit.ecal,
                                            30, 180, 210, hit.id);
    if(hit.id == 192) {
      t1 = hit.time;
      e1 = hit.ecal;      
      found1 = true;
    }
    if(hit.id == 193) {
      t2 = hit.time;
      e2 = hit.ecal;      
      found2 = true;
    }
 
  }
  if(found1 && found2) { 
    for(const auto& hit : event.labr.hits) {
      if(hit.id==192) continue;
      GHistogramer::Get().Fill("labr/dt_192_193",5000,-10,10,t1-t2,
                                         4000,0,8000,e1);
      if(1300 < e2 < 1360){
        GHistogramer::Get().Fill("labr/dt_192_193_1332",5000,-10,10,t1-t2,
                                                4000,0,8000,e1);
      }
    }
  }
}

