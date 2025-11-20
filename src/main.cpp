#include "file.h"
#include "rom.h"
#include <iostream>

#include <spdlog/spdlog.h>

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      std::cout << "USAGE: " << argv[0] << " <ROM_FILE>" << std::endl;
      return 1;
    }

  std::shared_ptr<spdlog::logger> logger = spdlog::default_logger ();
  spdlog::set_default_logger (logger);
  spdlog::set_pattern ("%l [%s:%#] %v");

#ifdef GLACEON_DEBUG
  spdlog::set_level (spdlog::level::debug);
  SPDLOG_DEBUG ("Debug logging enabled");
#endif

  GameROM rom = GameROM (argv[1]);

  return 0;
}
