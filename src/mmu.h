#ifndef GLACEON_MMU_H
#define GLACEON_MMU_H

#include "types.h"
#include <array>
#include <cstdint>
#include <semaphore>
#include <span>

/**
 * @brief A more readable format of the NDS memory chip select byte associated
 * with the ARM9 MCU.
 *
 * Refer to [this
 * site](https://blocksds.skylyrac.net/docs/technical/memory_map) for more
 * information.
 */
enum class ARM9MemoryDevice
{
  WRAM       = 0x02,
  SharedWRAM = 0x03,
  MMIO       = 0x04,
  VRAM       = 0x06,
  ROM        = 0x08,
  BIOS       = 0xFF,
};

template <uint32_t N> class AbstractWorkRAM
{
public:
  virtual void writeBytes (addr32_t addr, std::span<uint8_t> bytes) = 0;
  virtual uint8_t readByte (addr32_t addr)                          = 0;

protected:
  std::array<uint8_t, N> _raw;
};

class MainWRAM : public AbstractWorkRAM<0x3FFFFF>
{
public:
  MainWRAM ();

  void writeBytes (addr32_t addr, std::span<uint8_t> bytes) override;
  uint8_t readByte (addr32_t addr) override;
};

class SharedWRAM : AbstractWorkRAM<0x7FFF>
{
public:
  void writeBytes (addr32_t addr, std::span<uint8_t> bytes) override;
  uint8_t readByte (addr32_t addr) override;

private:
  std::binary_semaphore _semaphore;
};

/**
 * @brief This is a controller class for low-level memory management. Avoid
 * using other memory classes aside from this one.
 */
class MMU
{
public:
  /**
   * @brief Find the memory device to read/write to from a raw address.
   *
   * The high byte in a memory address acts as a chip select. This method
   * extracts this byte and returns an easily interpreted enum.
   *
   * @see MemorySelector
   */
  ARM9MemoryDevice decodeAddress (addr32_t addr);

private:
  MainWRAM _wram;
  SharedWRAM _shwram;
};

#endif
