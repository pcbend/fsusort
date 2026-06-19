

void simpleSort(const char *fname) {

  evtFile infile(fname);
  dataBlock db;

  GHistogramer::Get().SetOutFile("output.root");


  int counter=0;
  while(infile.ReadBlock(db)) {
    ddasHit hit;
    hit.set(db);
    GHistogramer::Get().Fill("summary",500,0,500,hit.GetId(),16000,0,64000,hit.GetEnergy());
  
    if(((counter++)%15000)==0) {
      printf(" %.03f / %.03f                            \r",infile.GetFilePos()/1024./1024.,infile.GetFileSize()/1024./1024.);
      fflush(stdout);
    }
  }
  printf("\n\n");
  GHistogramer::Get().Close();
}

