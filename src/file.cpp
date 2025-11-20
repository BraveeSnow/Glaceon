#include "file.h"

#include <iostream>

#include <spdlog/spdlog.h>

namespace
{

std::vector<FolderAllocationEntry>
parseFolderTable (std::ifstream &file, addr32_t tableOffset)
{
  std::vector<FolderAllocationEntry> folders;
  FolderAllocationEntry currentEntry;
  int count; // count found in root folder entry

  file.seekg (tableOffset, std::ios::beg);
  file.read (reinterpret_cast<char *> (&currentEntry),
             sizeof (FolderAllocationEntry));

  count = currentEntry.totalEntries - 1;
  folders.push_back (currentEntry);

  SPDLOG_DEBUG ("FNT folder count: {}", count);

  while (count > 0)
    {
      file.read (reinterpret_cast<char *> (&currentEntry),
                 sizeof (FolderAllocationEntry));
      folders.push_back (currentEntry);
      --count;
    }

  return folders;
}

std::vector<FileNameListEntry>
parseRootFolderList (std::ifstream &file, addr32_t tableOffset,
                     const FolderAllocationEntry &rootEntry)
{
  std::vector<FileNameListEntry> entries;
  FileNameListEntry currentEntry;
  uint8_t metadata;
  char buf[0b1111111];

  file.seekg (tableOffset + rootEntry.contentsOffset, std::ios::beg);

  for (int i = 0; i < rootEntry.totalEntries; i++)
    {
      // contains name size and isFolder bit
      file.read (reinterpret_cast<char *> (&metadata), sizeof (uint8_t));
      currentEntry.isFolder = (metadata & 0b10000000) != 0;
      file.read (buf, metadata & 0b01111111);
      currentEntry.name = std::string (buf, metadata & 0b01111111);

      // random zero byte padding? skip for now
      if (currentEntry.name.size () == 0)
        {
          continue;
        }

      // 16 bit value specific to folders only
      if (currentEntry.isFolder)
        {
          file.read (reinterpret_cast<char *> (&currentEntry.folderReference),
                     sizeof (uint16_t));
        }
      else
        {
          currentEntry.folderReference = 0;
        }

      entries.push_back (currentEntry);
    }

  return entries;
}

} // end namespace

FileNameTable::FileNameTable () : _folders () {}

FileNameTable::FileNameTable (std::ifstream &file, addr32_t tableOffset)
    : _folders (parseFolderTable (file, tableOffset))
{
  std::vector<FileNameListEntry> rootEntries
      = parseRootFolderList (file, tableOffset, _folders[0]);
}
