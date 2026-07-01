{

  //if(event.clarion.hits.size())
  //printf(RED "event.clarion.hits.size() = %lu" RESET_COLOR  "\n",event.clarion.hits.size()); 

  for(const auto& hit : event.trinity.hits) {

    GHistogramer::Get().Fill("trinity/summary",2000,0,64000, hit.total,
                                               150,50,200, 2*hit.id + 64);
 
    for(const auto& hit2 : event.trinity.hits) {
      double trinity_t,trinity_t2;
      if(hit.address==hit2.address) continue;
       GHistogramer::Get().Fill("trinity/hitmap",300,0,300,2*hit.id + 64,
                                                300,0,300,2*hit2.id + 64);
      trinity_t = hit.GetFastTime();
      trinity_t2 = hit2.GetFastTime();
      if(hit.total > 4000 && hit2.total > 4000){
        GHistogramer::Get().Fill("trinity/trinity_dt_summary",20000,-10000,10000,trinity_t-trinity_t2);
      }
      if(hit.id == 100 && hit2.id == 124 && hit.total > 4000 && hit2.total > 4000){
        GHistogramer::Get().Fill("trinity/dt_100_124",20000,-1000,1000,trinity_t-trinity_t2);
      }
    }

  
    for(const auto& chit : event.clarion.hits) {
      GHistogramer::Get().Fill("coinc/gt_timing",400,-200,200,hit.time - chit.time,
                                                4000, 0, 8000, chit.ecal);
      //}    
    }

    for(const auto& lhit : event.labr.hits) {
      if(hit.total > 4000) {
        GHistogramer::Get().Fill("coinc/trinity_labr_dt",20000,-10000,10000, hit.GetFastTime() - lhit.GetFastTime(),
                                                2000, 0, 8000, lhit.ecal);
      }    
    }

  }

  for(const auto& hit : event.clarion.hits) {
    GHistogramer::Get().Fill("clarion/summary",4000, 0, 8000, hit.ecal,
                                               70, 0, 70, hit.id);
    //if(charged_particle>0){
      //GHistogramer::Get().Fill("clarion/summary_onetrinity",4000, 0, 8000, hit.ecal,
      //                                         70, 0, 70, hit.id);
    //}

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

     //if(charged_particle>0){
       //GHistogramer::Get().Fill("clarion/gg_onetrinity",4000,0,8000,hit.ecal,
       //                                      4000,0,8000,hit2.ecal);
       //GHistogramer::Get().Fill("clarion/gg_onetrinity",4000,0,8000,hit2.ecal,
       //                                      4000,0,8000,hit.ecal);
    //}
    }
  }

  for(const auto& hit : event.labr.hits) {
     GHistogramer::Get().Fill("labr/summary_ecal",4000,0,8000,hit.ecal,
                                                  20,190,209,hit.id);
     
     if(hit.cfd == -5) continue;
     GHistogramer::Get().Fill("labr/cfd_value",2000,-10,10,hit.cfd,
                                                  20,190,209,hit.id);
    
    if(hit.timestamp < 0) continue; 
    if(hit.ecal < 30) continue;   


    for(const auto& chit : event.clarion.hits) {
      GHistogramer::Get().Fill("coinc/gl_timing",400,-200,200,hit.time - chit.time,
                                                4000, 0, 8000, chit.ecal);
      GHistogramer::Get().Fill("coinc/lg_timing",400,-200,200,hit.time - chit.time,
                                                4000, 0, 8000, hit.ecal);
    }

    for(const auto& hit2 : event.labr.hits) {
      if(hit.id == hit2.id) continue;
      if(hit2.cfd == -5) continue;
      if(hit2.timestamp < 0) continue;
      if(hit2.ecal < 30) continue;
      
      double e1,e2,t1,t2;
      e1 = hit.ecal;
      //e1 = hit.eqdc;
      t1 = hit.GetFastTime(); //I think this is in ns
      //t1 = hit.timestamp; //time is timestamp + cfd and is only correct for 100 MHz, 500 MHz is eqn 4-7. timestamp is in clock ticks (1 tick = 10 ns)
      e2 = hit2.ecal;
      //e2 = hit2.eqdc;
      t2 = hit2.GetFastTime(); //I think this is in ns
      //t2 = hit2.timestamp; //time is timestamp + cfd and is only correct for 100 MHz, 500 MHz is eqn 4-7. timestamp is in clock ticks (1 tick = 10 ns)

      //if(abs(hit.GetFastTime() - hit2.GetFastTime()) == 0. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 1. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 2. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 3. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 4. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 5. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 6. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 7. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 8. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 9. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 10. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 20. || abs(hit.GetFastTime() - hit2.GetFastTime()) == 30.){
        //GHistogramer::Get().Fill("labr/labr_intdifftime_dt",100,0,100,abs(hit.GetFastTime() - hit2.GetFastTime()),
        //                                                  1000,0,1000000000000,hit.GetFastTime());
        //GHistogramer::Get().Fill("labr/labr_intdifftime_e1_vs_e2",2000,0,8000,hit.ecal,
        //                                                  2000,0,8000,hit2.ecal);

        //continue; 
      //}

      //if(hit.cfd == hit2.cfd == 0){
        //GHistogramer::Get().Fill("labr/labr_cfd0_dt",100,0,100,abs(hit.GetFastTime() - hit2.GetFastTime()),
        //                                                  1000,0,1000000000000,hit.GetFastTime());
        //GHistogramer::Get().Fill("labr/labr_cfd0_e1_vs_e2",2000,0,8000,hit.ecal,
        //                                                  2000,0,8000,hit2.ecal);

        //continue;
      //}


      //GHistogramer::Get().Fill(Form("labr/dt_%i_%i",hit.id,hit2.id),10000,-100,100,t1-t2,2000,0,8000,e1);
      //GHistogramer::Get().Fill(Form("labr/dt_%i_%i",hit2.id,hit.id),10000,-100,100,t2-t1,2000,0,8000,e2);
      //GHistogramer::Get().Fill("labr/dt_sym_summary",10000,-100,100,t1-t2,2000,0,8000,e1);
      //GHistogramer::Get().Fill("labr/dt_sym_summary",10000,-100,100,t2-t1,2000,0,8000,e2);

      //current cal is a little off, gating for 1 hr 60Co run 223
      //if((1240 < e1) && (e1 < 1360)){
      //  GHistogramer::Get().Fill(Form("labr/dt_%i_%i_1332",hit2.id,hit.id),10000,-100,100,t2-t1,2000,0,8000,e2);
      //}
      //if((1240 < e2) && (e2 < 1360)){
      //  GHistogramer::Get().Fill(Form("labr/dt_%i_%i_1332",hit.id,hit2.id),10000,-100,100,t1-t2,2000,0,8000,e1);
      //}
        GHistogramer::Get().Fill(Form("labr/dt",hit2.id,hit.id),10000,-100,100,t1-t2,300,0,300,hit.id);



      GHistogramer::Get().Fill("labr/gg",2000,0,8000,hit.ecal,
                                             2000,0,8000,hit2.ecal);
      GHistogramer::Get().Fill("labr/gg",2000,0,8000,hit2.ecal,
                                             2000,0,8000,hit.ecal);

    }

    for(const auto& chit : event.clarion.hits) {
      GHistogramer::Get().Fill("coinc/labr_clarion_dt",20000,-10000,10000, hit.GetFastTime() - chit.GetFastTime(),
                                                4000, 0, 8000, chit.ecal);
    }


  }
}
