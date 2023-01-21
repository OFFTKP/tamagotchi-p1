#include "cpu.hxx"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/stacktrace.hpp>

void CPU::LoadRom(std::string filename) {
    std::vector<uint8_t> raw;
    std::ifstream ifs(filename);
    if (!ifs) {
        throw std::runtime_error("Could not open file");
    }
    ifs.seekg(0, std::ios::end);
    std::streampos length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    raw.resize(length);
    ifs.read(reinterpret_cast<char*>(raw.data()), length);
    // Convert bytes to nibbles
    memory_.resize(length / 2 * 3);
    if (memory_.size() != 6144 * 3) {
        throw std::runtime_error("Invalid ROM size");
    }
    for (size_t i = 0, j = 0; i < memory_.size();) {
        memory_[i] = raw[j] & 0xF;
        memory_[i + 1] = raw[j + 1] >> 4;
        memory_[i + 2] = raw[j + 1] & 0xF;
        i += 3;
        j += 2;
    }
    for (int i = 0; i < 0x880; i++) {
        log();
        step();
    }
}

bool CPU::check_c() {
    return F.Get() & 0b0001;
}

bool CPU::check_z() {
    return F.Get() & 0b0010;
}

bool CPU::check_i() {
    return F.Get() & 0b0100;
}

bool CPU::check_d() {
    return F.Get() & 0b1000;
}

void CPU::set_c(bool value) {
    F ^= (static_cast<int>(value ^ F.Get())) & (1UL << 0);
}

void CPU::set_z(bool value) {
    F ^= (static_cast<int>(value ^ F.Get())) & (1UL << 1);
}

void CPU::set_d(bool value) {
    F ^= (static_cast<int>(value ^ F.Get())) & (1UL << 2);
}

void CPU::set_i(bool value) {
    F ^= (static_cast<int>(value ^ F.Get())) & (1UL << 3);
}

Nibble CPU::read_rq(uint8_t addr) {
    switch (addr & 0b11) {
        case 0b00: return A;
        case 0b01: return B;
        case 0b10: return read_ram(IX);
        case 0b11: return read_ram(IY);
    }
    return 0;
}

void CPU::write_rq(uint8_t addr, Nibble value) {
    switch (addr & 0b11) {
        case 0b00: A = value; break;
        case 0b01: B = value; break;
        case 0b10: write_ram(IX, value); break;
        case 0b11: write_ram(IY, value); break;
    }
}

Nibble CPU::read(uint16_t addr) {
    return memory_.at(addr);
}

uint16_t CPU::get_addr() {
    uint16_t addr = PCB ? 0x1000 : 1;
    addr *= static_cast<int>(PCP.Get()) * 0x100;
    addr += PCS;
    // Each step counts as 3 nibbles
    addr *= 3;
    return addr;
}

uint16_t CPU::get_opcode() {
    uint16_t addr = get_addr();
    return read(addr).Get() << 8 | read(addr + 1).Get() << 4 | read(addr + 2).Get();
}

Nibble CPU::read_ram(uint16_t addr) {
    if (addr <= 0x27F) {
        return ram_[addr];
    } else if (addr >= 0xF00 && addr <= 0xF7E) {
        return read_io(addr);
    } else {
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        throw std::runtime_error("Tried to access invalid RAM address: " + std::to_string(static_cast<int>(addr)));
    }
}

void CPU::write_ram(uint16_t addr, Nibble value) {
    if (addr <= 0x27F) {
        ram_[addr] = value;
    } else if (addr >= 0xF00 && addr <= 0xF7E) {
        write_io(addr, value);
    } else {
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        throw std::runtime_error("Tried to access invalid RAM address: " + std::to_string(static_cast<int>(addr)));
    }
}

Nibble CPU::read_io(uint16_t addr) {
    std::cout << "Unimplemented io read from: " << std::hex << std::setfill('0') << std::setw(3) << addr << std::endl;
    return 0;
}

void CPU::write_io(uint16_t addr, Nibble value) {
    std::cout << "Unimplemented io write to: " << std::hex << std::setfill('0') << std::setw(3) << addr << std::endl;
}

void CPU::log() {
    auto opcode = get_opcode();
    std::cout << "Bank: " << static_cast<int>(PCB) << " Page: " << static_cast<int>(PCP.Get()) << " Step: " << static_cast<int>(PCS) << " Addr: ";
    std::cout << std::hex << std::setfill('0') << std::setw(3) << get_addr();
    std::cout << " " << std::hex << std::setfill('0') << std::setw(3) << opcode << " (" << serialize(decode(opcode)) << "): ";
    std::cout << "A: " << std::hex << std::setfill('0') << std::setw(1) << static_cast<int>(A.Get());
    std::cout << " B: " << std::hex << std::setfill('0') << std::setw(1) << static_cast<int>(B.Get());
    std::cout << " IX: " << std::hex << std::setfill('0') << std::setw(3) << static_cast<int>(IX);
    std::cout << " IY: " << std::hex << std::setfill('0') << std::setw(3) << static_cast<int>(IY);
    std::cout << " SP: " << std::hex << std::setfill('0') << std::setw(3) << static_cast<int>(SP);
    std::cout << std::endl;
}

void CPU::step() {
    opcode_ = get_opcode();
    OpcodeType type = decode(opcode_);
    auto cur_pcs = PCS;
    switch (type) {
        #define XMACRO(name, unused, ...) case OpcodeType::name: _##name(); break;
        #include "opcodes.def"
        #undef XMACRO
    }
    if (PCS == cur_pcs)
        PCS++;
}