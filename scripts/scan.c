

void scan(const char *fname,int end=100000) {

  evtFile infile(fname);
  dataBlock db;

  GChannel::ReadDetmap("cals/detmap.tsv");

  double lastTime = -1;

  int counter=0;
  while(infile.ReadBlock(db)>0) {
    ddasHit hit;
    hit.set(db);
  
    //hit.print();

    printf("0x%08x:\t %.01f \t %.01f\n",hit.GetAddress(),hit.GetTime(), hit.GetTime() - lastTime);

    lastTime = hit.GetTime();

    if(counter++ > end) break;
  }
}

