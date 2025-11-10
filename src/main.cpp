#include <iostream>

#include "rom.h"

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      std::cout << "USAGE: " << argv[0] << "<ROM_FILE>" << std::endl;
      return 1;
    }

  std::optional<ROMMainHeader> hdr = readMainHeader (argv[1]);
  return 0;
}
