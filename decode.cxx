#include "cpu.hxx"
#include <stdexcept>

struct instruction {
    constexpr instruction(uint16_t opcode, uint16_t mask) : Opcode(opcode), Mask(mask) {}
    uint16_t Opcode;
    uint16_t Mask;
};

constexpr instruction operator"" _i(const char* str, size_t size) {
    uint16_t mask = 0;
    int shift = 0;
    for (size_t i = 0; i < size; ++i, ++shift) {
        if (str[i] == '1' || str[i] == '0') {
            mask |= 0b1000'0000'0000 >> shift;
        }
        if (str[i] == '\'') {
            --shift;            
        }
    }
    uint16_t opcode = 0;
    shift = 0;
    for (size_t i = 0; i < size; ++i, ++shift) {
        if (str[i] == '1') {
            opcode |= 0b1000'0000'0000 >> shift;
        }
        if (str[i] == '\'') {
            --shift;            
        }
    }
    return instruction(opcode, mask);
}

#define XMACRO(name, mask, ...) constexpr instruction name##_mask = mask##_i;
#include "opcodes.def"
#undef XMACRO

OpcodeType CPU::decode(uint16_t opcode) {
    #define XMACRO(name, unused, ...) if ((opcode & name##_mask.Mask) == name##_mask.Opcode) return OpcodeType::name; else
    #include "opcodes.def"
    #undef XMACRO
    if (true) {
        std::string msg = "Unknown opcode: " + std::to_string(static_cast<int>(opcode));
        throw std::runtime_error(msg);
    }
}