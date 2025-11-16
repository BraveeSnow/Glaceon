#include "arm.h"

#include <cstring>

ARM9CPU::ARM9CPU (MMU &mmu) : _mmu (mmu) {}

/*****************************************************************************
 * ARM9 CPU Program Execution Management                                     *
 *****************************************************************************/

void
ARM9CPU::step ()
{
  _stepPipeline (_mmu.readByte (_programCounter));

  // step to next byte in memory
  _programCounter++;
}

void
ARM9CPU::jumpToAddress (addr32_t addr)
{
  _programCounter = addr;
}

/*****************************************************************************
 * ARM9 CPU CPSR State Management                                            *
 *****************************************************************************/

void
ARM9CPU::setProgramMode (ARM9ProgramMode mode)
{
  _cpsr &= 0xFFFFFFE0 + mode;
}

ARM9ProgramMode
ARM9CPU::getProgramMode ()
{
  return ARM9ProgramMode (_cpsr & 0b11111);
}

void
ARM9CPU::setCPSRState (uint32_t state, bool enable)
{
  _cpsr = enable ? _cpsr |= state : _cpsr &= ~state;
}

bool
ARM9CPU::getCPSRState (uint32_t state)
{
  return static_cast<bool> (_cpsr & state);
}

void
ARM9CPU::_stepPipeline (uint8_t code)
{
  // TODO: implement instruction pipeline
}

ARM9CompatLayer::ARM9CompatLayer () : _cpu (_mmu) {}
