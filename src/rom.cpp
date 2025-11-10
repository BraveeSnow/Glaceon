#include "rom.h"

#include <fstream>

std::optional<ROMMainHeader>
readMainHeader (std::string filePath)
{
  std::ifstream rom (filePath, std::ios::binary);
  ROMMainHeader hdr;

  if (!rom.is_open ())
    {
      return std::nullopt;
    }

  rom.read (reinterpret_cast<char *> (&hdr), sizeof (ROMMainHeader));
  if (rom.fail ())
    {
      return std::nullopt;
    }

  return hdr;
}
