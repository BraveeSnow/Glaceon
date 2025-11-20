/**
 * @file file.h
 * @brief File name table (FNT) and file allocation table (FAT) logic.
 *
 * The following documentation is crucial to understanding the structure of
 * FNT and FAT tables used in the NDS:
 * - https://ransei.link/research/file-formats/nds-file-system
 */

#ifndef GLACEON_FILE_H
#define GLACEON_FILE_H

#include "types.h"
#include <cstdint>
#include <fstream>
#include <vector>

#ifdef _WIN32
#pragma pack(push, 1)
#endif

struct FolderAllocationEntry
#ifndef _WIN32
    __attribute__ ((packed))
#endif
{
  addr32_t contentsOffset; // this is relative to the start of the FNT
  uint16_t firstIndex;

  union
  {
    uint8_t parentFolderIndex; // i have no idea what this is for
    uint16_t totalEntries;     // only applicable for root folder
  };
}; // size: 8 bytes

#ifdef _WIN32
#pragma pack(pop)
#endif

struct FileNameListEntry
{
  bool isFolder;
  std::string name;
  uint16_t folderReference; // value only used if folder
};

class FileNameTable
{
public:
  FileNameTable ();
  FileNameTable (std::ifstream &file, addr32_t tableOffset);

  FileNameTable &operator= (const FileNameTable &other) = default;

private:
  std::vector<FolderAllocationEntry> _folders;
};

#endif
