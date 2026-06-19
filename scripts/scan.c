

void simpleSort(const char *fname,int end=100) {

  evtFile infile(fname);
  dataBlock db;

  GChannel::ReadDetmap("cals/detmap.tsv");

  int counter=0;
  while(infile.ReadBlock(db)>0) {
    ddasHit hit;
    hit.set(db);
  
    hit.print();

    if(counter++ > end) break;
  }
}

