#ifndef GLACEON_ROM_H
#define GLACEON_ROM_H

#include "file.h"
#include "types.h"
#include <cstdint>
#include <fstream>
#include <optional>
#include <span>
#include <string>

struct ROMMainHeader
#ifdef _WIN32
#pragma pack(push, 1)
#else
    __attribute__ ((packed))
#endif
{
  char title[0xC];
  char id[0x4];
  char makerCode[0x2];
  uint8_t unitCode;
  uint8_t encryptSeed;
  uint8_t capacity;

  uint8_t _padding0x15[0x8]; // last bit used for DSi

  uint8_t region;
  uint8_t romVersion;
  uint8_t autostart;

  addr32_t arm9RomOffset;
  addr32_t arm9EntryAddr;
  addr32_t arm9RamAddr;
  uint32_t arm9Size;
  addr32_t arm7RomOffset;
  addr32_t arm7EntryAddr;
  addr32_t arm7RamAddr;
  uint32_t arm7Size;

  addr32_t fntOffset;
  uint32_t fntSize;
  addr32_t fatOffset;
  uint32_t fatSize;

  addr32_t arm9OverlayOffset;
  uint32_t arm9OverlaySize;
  addr32_t arm7OverlayOffset;
  uint32_t arm7OverlaySize;

  uint32_t normalCommands;
  uint32_t key1Commands;

  addr32_t iconTitleOffset;

  uint16_t saChecksum;
  uint16_t saDelay;

  addr32_t arm9AutoLoadList;
  addr32_t arm7AutoLoadList;

  uint64_t saDisable;
  uint32_t romTotalSize;
  uint32_t romHeaderSize;

  uint8_t _padding0x88[0xC]; // certain bits used for DSi

  // TODO: fill rest of fields when necessary
};

struct ROMOverlayEntry
#ifndef _WIN32
    __attribute__ ((packed))
#endif
{
  uint32_t id;
  addr32_t ramAddr;
  uint32_t ramSize;
  uint32_t bssSize;
  addr32_t staticInit;
  addr32_t staticEnd;
  uint32_t fileId;
  uint32_t _reserved;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

class GameROM
{
public:
  GameROM (const std::string filePath);

  FileNameTable getFileNameTable ();

  std::span<ROMOverlayEntry> getArm9Overlays ();
  std::span<ROMOverlayEntry> getArm7Overlays ();

private:
  std::ifstream _rom;
  std::unique_ptr<ROMMainHeader> _mainHeader;
  FileNameTable _fnt;
  std::span<ROMOverlayEntry> _arm9Overlays;
  std::span<ROMOverlayEntry> _arm7Overlays;
};

#endif // GLACEON_ROM_H
