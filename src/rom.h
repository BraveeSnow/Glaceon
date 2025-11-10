#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include <optional>
#include <string>

typedef unsigned int addr32_t;

#pragma pack(push, 1)

struct ROMMainHeader
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

#pragma pack(pop)

std::optional<ROMMainHeader> readMainHeader (std::string filePath);

#endif // ROM_H
