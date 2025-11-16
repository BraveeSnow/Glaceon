#ifndef GLACEON_ARM_H
#define GLACEON_ARM_H

#include <cstdint>

// ARM9 CPSR state bit masks
#define CPSR_INSTRUCTION_SET     1 << 4;
#define CPSR_FIQ_DISABLE         1 << 5;
#define CPSR_IRQ_DISABLE         1 << 6;
#define CPSR_ASYNC_ABORT_DISABLE 1 << 7;
#define CPSR_ENDIANNESS          1 << 8;

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
  ARM9CPU ();

  // CPSR state management
  void setProgramMode (ARM9ProgramMode mode);
  ARM9ProgramMode getProgramMode ();
  void setCPSRState (uint32_t state, bool enable);
  bool getCPSRState (uint32_t state);

private:
  void _initStack ();

  // --- CPU Configuration ---
  bool _thumbMode = false;

  // --- Internal Components ---
  uint32_t _registers[16];
  uint32_t &_stackPointer   = _registers[13];
  uint32_t &_linkRegister   = _registers[14];
  uint32_t &_programCounter = _registers[15];
  uint32_t _cpsr            = 0;
  uint32_t _spsr            = 0;

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
};

class ARM9CompatLayer
{
public:
  ARM9CompatLayer ();

private:
  ARM9CPU _cpu;
};

#endif
