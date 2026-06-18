
#include <GHistogramer.h>
#include <utils.h>
#include <TROOT.h>

GHistogramer& GHistogramer::Get() {
  static GHistogramer instance;
  return instance;
}

GHistogramer::GHistogramer() {
  fBaseDir = gROOT;
}

GHistogramer::~GHistogramer() = default;

void GHistogramer::Close() { 
  //for(auto& d : fDirCache)
  // if(auto *keys = d.second->GetListOfKeys()) keys->Sort();

  std::lock_guard<std::mutex> lock(fMutex);

  if(fBaseDir && fBaseDir->InheritsFrom(TFile::Class())) {
    TFile *file = static_cast<TFile*>(fBaseDir); //->Write();
    file->GetList()->Sort();
    file->Write();
    file->Close();
    //SetBaseDirectory("");
  }
  printf("histogramer closed.\n");
}

void GHistogramer::SetBaseDirectory(TDirectory* dir) { 
  std::lock_guard<std::mutex> lock(fMutex);
  fBaseDir = dir ? dir : gROOT;
  fDirCache.clear();
}

void GHistogramer::SetOutFile(const std::string& fname,const std::string& option) {
  TFile *outfile = new TFile(fname.c_str(),option.c_str());
  SetBaseDirectory(outfile);
}


TDirectory* GHistogramer::GetDirectory(const std::string& path) {
  if(!fBaseDir || path.empty()) return fBaseDir ? fBaseDir : gROOT;

  auto it = fDirCache.find(path);
  if(it!=fDirCache.end()) return it->second;

  TDirectory *dir = fBaseDir;
  auto parts = tokenizeString(path);
 
  for(const auto&p : parts) {
    TDirectory* sub = dir->GetDirectory(p.c_str());
    if(!sub) { 
      sub = dir->mkdir(p.c_str());
    }
    dir = sub;
  }
  fDirCache[path] = dir;
  return dir;
}

TH1* GHistogramer::Create(TDirectory* dir, const std::string& name,
                           int xbins,double xlow,double xhigh,
                           int ybins,double ylow,double yhigh) { 
  TDirectory* oldDir = gDirectory;
  if(dir) dir->cd();
  TH1* hist = 0;
  if(ybins>0) 
    hist = new TH2D(name.c_str(),name.c_str(),xbins,xlow,xhigh,ybins,ylow,yhigh);
  else
    hist = new TH1D(name.c_str(),name.c_str(),xbins,xlow,xhigh);
  hist->SetDirectory(dir);
  if(oldDir) oldDir->cd();
  return hist;
}

void GHistogramer::Fill(const std::string& pathName,        
                        int xbins,double xlow,double xhigh,double xvalue,       
                        int ybins,double ylow,double yhigh,double yvalue) {
  std::lock_guard<std::mutex> lock(fMutex);
  TH1* hist = 0;
  auto it = fH1.find(pathName);
  if(it==fH1.end()) {
    std::string path="";
    auto parts = tokenizeString(pathName);
    std::string name = parts.back();
    if(parts.size()>1) {
      path=parts[0];
      for(size_t i=1;i<parts.size()-1;i++) {
        path += "/";
        path += parts[i];
      }
    }
    TDirectory *dir = GetDirectory(path);
    hist = Create(dir,name,xbins,xlow,xhigh,ybins,ylow,yhigh);
    fH1[pathName] = hist;
  } else {
    hist = it->second;
  }
  if(ybins>0) 
    hist->Fill(xvalue,yvalue);
  else 
    hist->Fill(xvalue);
}


void GHistogramer::Print(Option_t *opt) const {
  int counter =0;
  for(auto item : fH1) {
    printf("%i:    {%s,0x%p}\n",counter++,item.first.c_str(),item.second);
  }

}






