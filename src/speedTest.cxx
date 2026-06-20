
#include <iostream>
#include <chrono>

#include <evtFile.h>
#include <dataBlock.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cerr << "usage: readSpeedTest file.evt\n";
    return 1;
  }

  evtFile file(argv[1], false);
  dataBlock block;

  uint64_t nblock = 0;
  auto start = std::chrono::steady_clock::now();

  while(true) {
    int status = file.ReadBlock(block, 0);

    if(status == 1) {
      nblock++;
      continue;
    }

    if(status == -2)
      continue;

    break;
  }

  auto stop = std::chrono::steady_clock::now();
  double sec = std::chrono::duration<double>(stop - start).count();

  double mb = file.GetFilePos() / 1024.0 / 1024.0;

  std::cout << "blocks = " << nblock << "\n";
  std::cout << "MB     = " << mb << "\n";
  std::cout << "sec    = " << sec << "\n";
  std::cout << "MB/s   = " << mb / sec << "\n";

  return 0;
}

