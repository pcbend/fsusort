#ifndef __GHISTOGRAMER_H__
#define __GHISTOGRAMER_H__


#include<string>
#include<map>
#include<mutex>
#include<cmath>

#include <TFile.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TH2.h>


class GHistogramer {
  public:
    static GHistogramer& Get();
    
  private:
    GHistogramer();
    GHistogramer(const GHistogramer&)            = delete;
    GHistogramer& operator=(const GHistogramer&) = delete;
    ~GHistogramer();
    
  public: //private:
    void Close();
    void Print(Option_t *opt="") const;

    void Fill(const std::string& pathName,
              int xbins,double xlow,double xhigh,double xvalue,
              int ybins=-1,double ylow=sqrt(-1),double yhigh=sqrt(-1),double yvalue=sqrt(-1));

    TH1* Create(TDirectory* dir, const std::string& name, 
              int xbins,double xlow,double xhigh,
              int ybins=-1,double ylow=sqrt(-1),double yhigh=sqrt(-1));

    TDirectory* GetDirectory(const std::string& path);

    std::string makeKey(const std::string& path, const std::string& name) const {
      if (path.empty()) return name;
      return path + "/" + name;
    }

    void SetBaseDirectory(TDirectory *dir);
    void SetOutFile(const std::string& fname="output.root",const std::string& option="recreate");


  private:
    std::mutex fMutex; 
    std::unordered_map<std::string,TH1*> fH1; 
    std::unordered_map<std::string, TDirectory*> fDirCache;
    TDirectory* fBaseDir  = nullptr;

  ClassDef(GHistogramer,0)  
};



#endif
