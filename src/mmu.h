#ifndef GLACEON_MMU_H
#define GLACEON_MMU_H

#include "types.h"
#include <array>
#include <cstdint>
#include <shared_mutex>
#include <span>

#define MAIN_WRAM_SIZE   0x400000
#define SHARED_WRAM_SIZE 0x8000

/**
 * @brief A more readable format of the NDS memory chip select byte associated
 * with both the ARM9 and ARM7 memory maps.
 *
 * Refer to [this
 * site](https://blocksds.skylyrac.net/docs/technical/memory_map) for more
 * information.
 */
enum class MemoryDevice
{
  WRAM       = 0x02,
  SharedWRAM = 0x03,
  ARM7WRAM   = 0x038,
  MMIO       = 0x04,
  PALETTE    = 0x05,
  VRAM       = 0x06,
  OAM        = 0x07,
  ROM        = 0x08,
  BIOS       = 0xFF,
};

template <uint32_t N> class AbstractRAM
{
public:
  virtual void writeBytes (addr32_t addr, std::span<uint8_t> bytes) = 0;
  virtual uint8_t readByte (addr32_t addr)                          = 0;

protected:
  std::array<uint8_t, N> _raw;
  std::shared_mutex _mutex;
};

class MainWRAM : public AbstractRAM<MAIN_WRAM_SIZE>
{
public:
  MainWRAM ();

  void writeBytes (addr32_t addr, std::span<uint8_t> bytes) override;
  uint8_t readByte (addr32_t addr) override;
};

class SharedWRAM : AbstractRAM<SHARED_WRAM_SIZE>
{
public:
  SharedWRAM ();

  void writeBytes (addr32_t addr, std::span<uint8_t> bytes) override;
  uint8_t readByte (addr32_t addr) override;
};

/**
 * @brief This is a controller class for low-level memory management. Avoid
 * using other memory classes aside from this one.
 */
class MMU
{
public:
  MMU ();

  uint8_t readByte (addr32_t addr);

  /**
   * @brief Find the memory device to read/write to from a raw address.
   *
   * The high byte in a memory address acts as a chip select. This method
   * extracts this byte and returns an easily interpreted enum.
   *
   * @see MemorySelector
   */
  MemoryDevice decodeAddress (addr32_t addr);

private:
  MainWRAM _wram;
  SharedWRAM _shwram;
};

#endif
