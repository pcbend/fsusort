{

  double labr_fasttime =-1;
  double trinity_fasttime =-1;
  double clarion_fasttime =-1;

  for(auto hit : hits) {
    
    //if(hit.GetId()>=202 && hit.GetId() <=206) {
      //if(time1<0) time1 = hit.GetTime();
      //if(time1>0) time2 = hit.GetTime();
    //}


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
        if(hit.GetCFD() == -5) continue;
        if(labr_fasttime < 0){
          labr_fasttime = (hit.GetTimestamp()*5 + hit.GetCFD())*2;
        }         
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

        if(hit.GetCFD() == -5) continue;
        if(total < 4000) continue;
        if(trinity_fasttime < 0){
          trinity_fasttime = (hit.GetTimestamp() + hit.GetCFD())*10;
        }
      }
    }



  if(hit.GetId()==1 || hit.GetId()==2 || hit.GetId()==3 || hit.GetId()==10 || hit.GetId()==11 || hit.GetId()==12 || hit.GetId()==13 || hit.GetId()==16 || hit.GetId()==18 || hit.GetId()==19 || hit.GetId()==21 || hit.GetId()==22 || hit.GetId()==23 || hit.GetId()==33 || hit.GetId()==34 || hit.GetId()==37 ||  hit.GetId()==38 || hit.GetId()==40 || hit.GetId()==42 || hit.GetId()==43 || hit.GetId()==44 || hit.GetId()==48 || hit.GetId()==49 || hit.GetId()==50 || hit.GetId()==51){  //Clarion
    if(trinity_fasttime > 0){
      GHistogramer::Get().Fill("clarion_trinity",4000,0,8000,hit.GetEcal(),
                                                  70,0,70,hit.GetId());
      clarion_fasttime = (hit.GetTimestamp() + hit.GetCFD())*10;
    }  
  }
}  

  if(labr_fasttime>0 && trinity_fasttime>0) {
   GHistogramer::Get().Fill("dt_labr_trinity",20000,-10000,10000,labr_fasttime-trinity_fasttime);
  }	  

  if(labr_fasttime>0 && clarion_fasttime>0) {
   GHistogramer::Get().Fill("dt_labr_clarion",20000,-10000,10000,labr_fasttime-clarion_fasttime);
  }

  if(clarion_fasttime>0 && trinity_fasttime>0) {
   GHistogramer::Get().Fill("dt_clarion_trinity",20000,-10000,10000,clarion_fasttime-trinity_fasttime);
  }


}



