#include "rom.h"

#include <vector>

namespace
{

std::unique_ptr<ROMMainHeader>
readMainHeader (std::ifstream &file)
{
  ROMMainHeader hdr;

  file.seekg (0, std::ios::beg);
  file.read (reinterpret_cast<char *> (&hdr), sizeof (ROMMainHeader));

  if (file.fail ())
    {
      throw std::exception ();
    }

  return std::make_unique<ROMMainHeader> (hdr);
}

std::span<ROMOverlayEntry>
readOverlayEntries (std::ifstream &file, addr32_t headerOffset,
                    uint32_t headerSize)
{
  std::vector<ROMOverlayEntry> overlays;
  ROMOverlayEntry rawOverlay;

  if (headerSize == 0)
    {
      return overlays;
    }

  file.seekg (headerOffset, std::ios::beg);
  for (int i = 0; i < headerSize / sizeof (ROMOverlayEntry); i++)
    {
      file.read (reinterpret_cast<char *> (&rawOverlay),
                 sizeof (ROMOverlayEntry));
    }

  return overlays;
}

} // end namespace

GameROM::GameROM (const std::string filePath)
{
  _rom = std::ifstream (filePath, std::ios::binary);

  if (!_rom.is_open ())
    {
      throw std::exception ();
    }

  _mainHeader = readMainHeader (_rom);
  _fnt.readFrom (_rom, _mainHeader->fntOffset);
  _arm9Overlays = readOverlayEntries (_rom, _mainHeader->arm9OverlayOffset,
                                      _mainHeader->arm9OverlaySize);
  _arm7Overlays = readOverlayEntries (_rom, _mainHeader->arm7OverlayOffset,
                                      _mainHeader->arm7OverlaySize);
}

std::span<ROMOverlayEntry>
GameROM::getArm9Overlays ()
{
  return _arm9Overlays;
}

std::span<ROMOverlayEntry>
GameROM::getArm7Overlays ()
{
  return _arm7Overlays;
}
