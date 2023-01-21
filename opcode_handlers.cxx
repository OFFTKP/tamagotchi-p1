#include "cpu.hxx"
#include <iostream>

#define XMACRO(name, unused, ...) void CPU::_##name() { __VA_ARGS__ }
#include "opcodes.def"
#undef XMACRO