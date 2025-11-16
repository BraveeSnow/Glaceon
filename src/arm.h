#ifndef GLACEON_ARM_H
#define GLACEON_ARM_H

#include "mmu.h"
#include <cstdint>
#include <queue>

// ARM9 CPSR state bit masks
#define CPSR_INSTRUCTION_SET     1 << 4;
#define CPSR_FIQ_DISABLE         1 << 5;
#define CPSR_IRQ_DISABLE         1 << 6;
#define CPSR_ASYNC_ABORT_DISABLE 1 << 7;
#define CPSR_ENDIANNESS          1 << 8;

#define CPSR_BIT_OVERFLOW 1 << 28;
#define CPSR_BIT_CARRY    1 << 29;
#define CPSR_BIT_ZERO     1 << 30;
#define CPSR_BIT_NEGATIVE 1 << 31;

enum ARM9ProgramMode
{
  User       = 0b10000,
  FIQ        = 0b10001,
  IRQ        = 0b10010,
  Supervisor = 0b10011,
  Abort      = 0b10111,
  Undefined  = 0b11011,
  System     = 0b11111,
};

class ARM9CPU
{
public:
  explicit ARM9CPU (MMU &mmu);

  // --- Program Execution Management ---
  void step ();
  void jumpToAddress (addr32_t addr);

  // --- CPSR State Management ---
  void setProgramMode (ARM9ProgramMode mode);
  ARM9ProgramMode getProgramMode ();
  void setCPSRState (uint32_t state, bool enable);
  bool getCPSRState (uint32_t state);

private:
  void _initStack ();

  /**
   * @brief Steps the instruction pipeline with the supplied instruction code.
   *
   * The instruction pipeline should have a size of 3 bytes (aside from initial
   * execution). The pipeline will first run through the current set of bytes,
   * discard the last byte and emplace the new byte at the beginning of the
   * queue. Do **not** manually emplace bytes into the instruction pipeline
   * directly; use this function instead.
   *
   * TODO: The pipeline itself should be contained within a separate class and
   * handle the CPU state after instruction execution.
   *
   * @param code The instruction code to step the pipeline with.
   */
  void _stepPipeline (uint8_t code);

  // --- CPU Configuration ---
  bool _thumbMode = false;

  // --- Internal Components ---
  uint32_t _registers[16];
  uint32_t &_stackPointer   = _registers[13];
  uint32_t &_linkRegister   = _registers[14];
  uint32_t &_programCounter = _registers[15];
  uint32_t _cpsr            = 0;
  uint32_t _spsr            = 0;

  std::queue<uint8_t> _instructionPipeline;

  /**
   * Banked registers are associated with the following privileged runtime
   * modes, in this specific order:
   *
   * FIQ, IRQ, SVC, ABT, UND.
   *
   * The only exception to this are the general-purpose registers R8-R12, which
   * are only used in FIQ mode.
   *
   * @see ARM9ProgramMode for all available runtime modes.
   */
  struct _bank
  {
    uint32_t _fiqRegisters[5]; // TODO: maybe move this outside of the bank
    uint32_t _stackPointer[5];
    uint32_t _linkRegister[5];
    uint32_t _spsr[5];
  };

  // --- External Components ---
  MMU &_mmu;
};

class ARM9CompatLayer
{
public:
  ARM9CompatLayer ();

private:
  ARM9CPU _cpu;
  MMU _mmu;
};

#endif
