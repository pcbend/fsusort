{

  double time1 =-1;
  double time2 =-1;

  for(auto hit : hits) {
    
    if(hit.GetId()>=202 && hit.GetId() <=206) {
      if(time1<0) time1 = hit.GetTime();
      if(time1>0) time2 = hit.GetTime();
    }


    GHistogramer::Get().Fill("ecal",4000,0,8000,hit.GetEcal(),
                                       300,0,300,hit.GetId());
    GHistogramer::Get().Fill("raw",16000,0,16000,hit.GetCharge(),
                                       300,0,300,hit.GetId());
    
    
    if(hit.GetId()>=192) {
      if(hit.GetForcedCFD()) { 
    	GHistogramer::Get().Fill(Form("Forced_%i",hit.GetId()),10,0,10,2,4000,0,8000,hit.GetEcal_labr());
      } else {
    	GHistogramer::Get().Fill(Form("Forced_%i",hit.GetId()),10,0,10,4,4000,0,8000,hit.GetEcal_labr());
      }
    }

    if(hit.GetId()>=192 && hit.GetId()<=207) {  //fast timing
      if(hit.hasQDC && !hit.GetForcedCFD()) { 
        double add = hit.GetQDC()[2] + hit.GetQDC()[3]; 
        double sub = hit.GetQDC()[1];             
        GHistogramer::Get().Fill("QDCsummary",16000,0,0,add-sub*(140./100.) ,   // zero-zero are auto limits
                                              300,0,300,hit.GetId());
      }
    }
    
    if(hit.GetId()>=64 && hit.GetId()<=191) { //trinity 
      if(hit.hasQDC && !hit.GetForcedCFD()) { 
        double base  = hit.GetQDC()[0]+hit.GetQDC()[1];
        double peak  = hit.GetQDC()[3] - (base)*(20./45.);
        double tail  = hit.GetQDC()[5] - (base)*(55./45.);
        double total = (hit.GetQDC()[3]+hit.GetQDC()[4]+hit.GetQDC()[5]) - base*(85./45);

        GHistogramer::Get().Fill("QDCsummary",16000,0,0,total,   // zero-zero are auto limits
                                              300,0,300,hit.GetId());
        
        
        GHistogramer::Get().Fill("QDCRatio",16000,0,0,tail/peak,   // zero-zero are auto limits
                                            300,0,300,hit.GetId());
     
       
        GHistogramer::Get().Fill(Form("trinity/det%03i",hit.GetId()),1000,0,1,tail/peak,
                                                                     2000,0,64000,total);

      }
    }
  }

  if(time1>0 && time2>0 && time1!=time2) {
   GHistogramer::Get().Fill("dt",20000,-100,100,time1-time2);
  }	  

}



