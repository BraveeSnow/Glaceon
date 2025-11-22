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
#include <map>
#include <span>
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
    uint8_t parentFolderIndex;
    uint16_t totalEntries; // only applicable for root folder
  };
}; // size: 8 bytes

#ifdef _WIN32
#pragma pack(pop)
#endif

class AbstractFSEntry
{
public:
  AbstractFSEntry (const std::string &name, bool isDirectory) noexcept;
  virtual ~AbstractFSEntry () = default;

  bool isDirectory () const noexcept;
  const std::string &getName () const noexcept;

protected:
  const std::string _name;
  const bool _isDirectory;
};

class FileEntry : public AbstractFSEntry
{
public:
  FileEntry (const std::string &name) noexcept;
  FileEntry (const std::string &name, std::span<const uint8_t> data) noexcept;

  std::span<const uint8_t> getData () const noexcept;
  void setData (std::span<const uint8_t> data) noexcept;

private:
  std::vector<uint8_t> _data;
};

class DirectoryEntry : public AbstractFSEntry
{
public:
  DirectoryEntry (const std::string &name, uint16_t folderRef) noexcept;

  bool addEntry (const AbstractFSEntry &child) noexcept;
  const std::unique_ptr<AbstractFSEntry> &
  getEntry (std::string &name) const noexcept;
  const uint16_t getFolderReference ();

protected:
  std::map<const std::string, const std::unique_ptr<AbstractFSEntry>>
      _children;

private:
  uint16_t _folderRef;
};

struct FileNameListEntry
{
  bool isDirectory;
  std::string name;
  uint16_t folderReference; // value only used if folder
};

class NitroFS : public DirectoryEntry
{
public:
  NitroFS ();
  NitroFS (std::ifstream &file, addr32_t tableOffset);

  void readFrom (std::ifstream &file, addr32_t tableOffset);

private:
  std::map<std::string, AbstractFSEntry> _root;
  std::vector<FolderAllocationEntry> _folderAllocationEntries;
};

#endif
