#include "file.h"

#include <iostream>

#include <spdlog/spdlog.h>

#define FNT_FOLDER_MASK 0b10000000
#define FNT_NAME_LENGTH 0b01111111

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
readFolderNameList (std::ifstream &file, addr32_t tableOffset,
                    const FolderAllocationEntry &folderEntry)
{
  std::vector<FileNameListEntry> entries;
  FileNameListEntry currentEntry;
  uint8_t metadata;
  char buf[FNT_NAME_LENGTH];

  file.seekg (tableOffset + folderEntry.contentsOffset, std::ios::beg);

  do
    {
      metadata                 = file.get ();
      currentEntry.isDirectory = (metadata & FNT_FOLDER_MASK) != 0;
      file.read (buf, metadata & FNT_NAME_LENGTH);
      currentEntry.name = std::string (buf, metadata & FNT_NAME_LENGTH);

      if (currentEntry.isDirectory)
        {
          file.read (reinterpret_cast<char *> (&currentEntry.folderReference),
                     sizeof (uint16_t));
          currentEntry.folderReference
              -= 0xF000; // folder index from allocation table can be found by
                         // subtracting 0xF000
        }
      else
        {
          currentEntry.folderReference = 0;
        }

      entries.push_back (currentEntry);
    }
  while (file.peek () != 0); // list terminates at zero byte

  return entries;
}

std::map<std::string, AbstractFSEntry>
mapFolderContents (const FolderAllocationEntry &folder,
                   const std::vector<FileNameListEntry> &nameList)
{
  std::map<std::string, AbstractFSEntry> entries;

  for (const FileNameListEntry &entry : nameList)
    {
      if (entry.isDirectory)
        {
          entries.try_emplace (
              entry.name, DirectoryEntry (entry.name, entry.folderReference));
        }
      else
        {
          entries.emplace (entry.name, FileEntry (entry.name));
        }
    }

  return entries;
}

} // end namespace

/*****************************************************************************
 * ABSTRACT NITROFS ENTRY DEFINITIONS                                        *
 *****************************************************************************/

AbstractFSEntry::AbstractFSEntry (const std::string &name,
                                  bool isDirectory) noexcept
    : _name (name), _isDirectory (isDirectory)
{
}

bool
AbstractFSEntry::isDirectory () const noexcept
{
  return _isDirectory;
}

const std::string &
AbstractFSEntry::getName () const noexcept
{
  return _name;
}

/*****************************************************************************
 * NITROFS FILE ENTRY CLASS DEFINITIONS                                      *
 *****************************************************************************/

FileEntry::FileEntry (const std::string &name) noexcept : FileEntry (name, {})
{
}

FileEntry::FileEntry (const std::string &name,
                      std::span<const uint8_t> data) noexcept
    : AbstractFSEntry (name, false), _data (data.begin (), data.end ())
{
}

std::span<const uint8_t>
FileEntry::getData () const noexcept
{
  return std::span<const uint8_t> (_data);
}

void
FileEntry::setData (std::span<const uint8_t> data) noexcept
{
  _data = std::vector<uint8_t> (data.begin (), data.end ());
}

/*****************************************************************************
 * NITROFS DIRECTORY ENTRY CLASS DEFINITIONS                                 *
 *****************************************************************************/

DirectoryEntry::DirectoryEntry (const std::string &name,
                                uint16_t folderRef) noexcept
    : AbstractFSEntry (name, true), _children (), _folderRef (folderRef)
{
}

bool
DirectoryEntry::addEntry (const AbstractFSEntry &child) noexcept
{
  auto [_, emplaced] = _children.try_emplace (
      child.getName (), std::make_unique<AbstractFSEntry> (child));
  return emplaced;
}

const std::unique_ptr<AbstractFSEntry> &
DirectoryEntry::getEntry (std::string &name) const noexcept
{
  return _children.at (name);
}

/*****************************************************************************
 * NITROFS FILESYSTEM MANAGEMENT                                             *
 *****************************************************************************/

NitroFS::NitroFS () : DirectoryEntry ("/", 0), _root () {}

NitroFS::NitroFS (std::ifstream &file, addr32_t tableOffset)
    : DirectoryEntry ("/", 0)
{
  readFrom (file, tableOffset);
}

void
NitroFS::readFrom (std::ifstream &file, addr32_t tableOffset)
{
  std::vector<FileNameListEntry> names;

  _folderAllocationEntries = parseFolderTable (file, tableOffset);

  for (const FolderAllocationEntry &folder : _folderAllocationEntries)
    {
      // TODO: implement recursive folder reading
      names = readFolderNameList (file, tableOffset, folder);
      mapFolderContents (folder, names);
    }
}
