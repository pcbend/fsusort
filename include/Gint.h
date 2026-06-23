#ifndef __GINT_H__
#define __GINT_H__

#ifndef __CINT__
#include <thread>
#endif

#include <TRint.h>
#include <TSysEvtHandler.h>

#include <Gtypes.h>
#include <globals.h>

class TFile;

#ifndef __CINT__

#include <unistd.h>
#include <fcntl.h>

class GintOptions {
  private:
    GintOptions() = default;
  public:
    ~GintOptions() = default;

    static void AddGEBFile(std::string fname)     { fGEBFiles.push_back(fname); }
    static std::vector<std::string> GetGEBFiles() { return fGEBFiles; }

    static void AddEVTFile(std::string fname)     { fEVTFiles.push_back(fname); }
    static std::vector<std::string> GetEVTFiles() { return fEVTFiles; }

    static void SetQuitAfterSort(bool val)  { fQuitAfterSort = val; }
    static bool QuitAfterSort()             { return fQuitAfterSort; }

  private:
    static std::vector<std::string> fGEBFiles;
    static std::vector<std::string> fEVTFiles;
    static bool fQuitAfterSort;

};

class GSigHandler : public TSignalHandler {
  public:
    GSigHandler() : TSignalHandler(kSigInterrupt,false) {} 

    bool Notify() override {
      static std::atomic<int> count{0};
      int n = ++count;
      if(n==1) { 
        gShutdown.store(true);
    
        /*
        // Redirect stdout + stderr to /dev/null
        int devnull = ::open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
          ::dup2(devnull, STDOUT_FILENO);
          ::close(devnull);
        }
        */
        return true;
      }
      _exit(1);
      return true;
    }
};
#endif

class Gint : public TRint {
  private:
    Gint(int argc, char **argv);
    static Gint *fGint; 
  public:
    static Gint *Get(int argc=0,char **argv=0);
    virtual ~Gint();
  
    int  TabCompletionHook(char* buf, int* pLoc, std::ostream& out) override;
    long ProcessLine(const char *line, bool sync=true,int *error=0) override;

    void Terminate(int status) override;

    void Sort(std::string fname);


  public:
    void      LoadOptions(int argc, char **argv);
    void      ApplyOptions(); 
    //bool      FileAutoDetect(const std::string& filename);
    TFile*    OpenRootFile(const std::string& filename, Option_t *opt="");

    void      LoadStyle();    

  private:
    int fRootFilesOpened;
    bool fTabLock;

#ifndef __CINT__
    std::thread::id fMainThreadId;
#endif

  ClassDefOverride(Gint,0)
};



#endif

