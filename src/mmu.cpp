#include "mmu.h"

MainWRAM::MainWRAM () {}

void
MainWRAM::writeBytes (addr32_t addr, std::span<uint8_t> bytes)
{
  std::unique_lock lock (_mutex);
  for (int i = 0; i < bytes.size (); i++)
    {
      _raw[i + addr] = bytes[i];
    }
}

uint8_t
MainWRAM::readByte (addr32_t addr)
{
  std::shared_lock lock (_mutex);
  return _raw[addr];
}

SharedWRAM::SharedWRAM () {}

void
SharedWRAM::writeBytes (addr32_t addr, std::span<uint8_t> bytes)
{
  std::unique_lock lock (_mutex);
  for (int i = 0; i < bytes.size (); i++)
    {
      _raw[i + addr] = bytes[i];
    }
}

uint8_t
SharedWRAM::readByte (addr32_t addr)
{
  std::shared_lock lock (_mutex);
  uint8_t byte;

  byte = _raw[addr];

  return byte;
}

MMU::MMU () {}

uint8_t
MMU::readByte (addr32_t addr)
{
  MemoryDevice device = decodeAddress (addr);
  addr &= 0xFFFFFF;

  switch (device)
    {
    case MemoryDevice::WRAM:
      return _wram.readByte (addr);
    case MemoryDevice::SharedWRAM:
      return _shwram.readByte (addr);
    default:
      throw std::out_of_range ("MMU::readByte: Invalid memory device");
    }
}

MemoryDevice
MMU::decodeAddress (addr32_t addr)
{
  if ((addr >> 20 & 0xFFF) == 0x038)
    {
      return MemoryDevice::ARM7WRAM;
    }

  return MemoryDevice ((addr >> 24) & 0xFF);
}
