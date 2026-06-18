
#include<cstdio>
#include<signal.h>


#include<Gint.h>
#include<utils.h>
#include<globals.h>

#include<TROOT.h>
#include<TEnv.h>

void SigHandler(int) { 
  gShutdown.store(true);
}

void loadEnv() {
  setenv("FSUSORTSYS", (programPath()+"/..").c_str(), 0);

  printf("FSUSORTSYS has been set to: %s\n",getenv("FSUSORTSYS"));

  // Load $GSORTSYS/.gsortrc
  std::string fsusortrc_path = Form("%s/.fsusortrc",getenv("FSUSORTSYS"));
  gEnv->ReadFile(fsusortrc_path.c_str(),kEnvChange);

  // Load $HOME/.gsortrc
  fsusortrc_path = Form("%s/.fsusortrc",getenv("HOME"));
  if(fileExists(fsusortrc_path.c_str())){
    gEnv->ReadFile(fsusortrc_path.c_str(),kEnvChange);
  }
  
  fsusortrc_path = Form("%s/.fsusortrc",getenv("PWD"));
  if(fileExists(fsusortrc_path.c_str())){
    gEnv->ReadFile(fsusortrc_path.c_str(),kEnvChange);
  }
}


int main(int argc, char **argv) {

  ROOT::EnableThreadSafety();

  //loadStyle();
  loadEnv();
  gROOT->ProcessLine(".L include/Gtypes.h"); // for some reason the enums aren't being processed properly by the rootclang

  //signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);
  
  Gint::Get(argc,argv)->Run(true);
 
  return 0;
}
