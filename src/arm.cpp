#include "arm.h"

#include <cstring>

ARM9CPU::ARM9CPU () {}

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

ARM9CompatLayer::ARM9CompatLayer () {}
