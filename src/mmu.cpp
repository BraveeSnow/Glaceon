#include "mmu.h"

void
MainWRAM::writeBytes (addr32_t addr, std::span<uint8_t> bytes)
{
  for (int i = 0; i < bytes.size (); i++)
    {
      _raw[i + addr] = bytes[i];
    }
}

uint8_t
MainWRAM::readByte (addr32_t addr)
{
  return _raw[addr];
}

void
SharedWRAM::writeBytes (addr32_t addr, std::span<uint8_t> bytes)
{
  _semaphore.try_acquire ();

  for (int i = 0; i < bytes.size (); i++)
    {
      _raw[i + addr] = bytes[i];
    }

  _semaphore.release ();
}

uint8_t
SharedWRAM::readByte (addr32_t addr)
{
  uint8_t byte;

  _semaphore.try_acquire ();
  byte = _raw[addr];
  _semaphore.release ();

  return byte;
}

ARM9MemoryDevice
MMU::decodeAddress (addr32_t addr)
{
  return ARM9MemoryDevice ((addr >> 24) & 0xFF);
}
