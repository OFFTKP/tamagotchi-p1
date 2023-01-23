#include "cpu.hxx"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <boost/stacktrace.hpp>

// Mappings from TamaLib
static uint8_t seg_pos[40] = {
    0, 1, 2, 3, 4, 5, 6, 7, 32,
    8, 9, 10, 11, 12, 13, 14, 15,
    33, 34, 35,
    31, 30, 29, 28, 27, 26, 25, 24,
    36,
    23, 22, 21, 20, 19, 18, 17, 16,
    37, 38, 39
};

void CPU::LoadRom(std::string filename) {
    reset();
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
}

void CPU::Step() {
    step();
}

void CPU::KeyPress(int key) {
    keys_pressed_[key] = true;
}

void CPU::KeyRelease(int key) {
    keys_pressed_[key] = false;
}

const std::array<uint8_t, 32 * 16>& CPU::GetDisplay() {
    return screen_;
}

bool CPU::check_c() {
    return F.Get() & 0b0001;
}

bool CPU::check_z() {
    return F.Get() & 0b0010;
}

bool CPU::check_d() {
    return F.Get() & 0b0100;
}

bool CPU::check_i() {
    return F.Get() & 0b1000;
}

void CPU::set_c(bool value) {
    F ^= (-static_cast<unsigned int>(value) ^ F.Get()) & (1UL << 0);
}

void CPU::set_z(bool value) {
    F ^= (-static_cast<unsigned int>(value) ^ F.Get()) & (1UL << 1);
}

void CPU::set_d(bool value) {
    F ^= (-static_cast<unsigned int>(value) ^ F.Get()) & (1UL << 2);
}

void CPU::set_i(bool value) {
    F ^= (-static_cast<unsigned int>(value) ^ F.Get()) & (1UL << 3);
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
    uint16_t addr = PCB ? 0x1000 : 0;
    addr += static_cast<int>(PCP.Get()) * 0x100;
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
    } else if (addr >= 0xE00 && addr <= 0xE4F) {
        return read_vram(addr);
    } else if (addr >= 0xE80 && addr <= 0xECF) {
        return read_vram(addr - 0x30);
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
    } else if (addr >= 0xE00 && addr <= 0xE4F) {
        return write_vram(addr, value);
    } else if (addr >= 0xE80 && addr <= 0xECF) {
        return write_vram(addr, value);
    } else if (addr >= 0xF00 && addr <= 0xF7E) {
        write_io(addr, value);
    } else {
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        throw std::runtime_error("Tried to access invalid RAM address: " + std::to_string(static_cast<int>(addr)));
    }
}

Nibble CPU::read_vram(uint16_t addr) {
    return 0;
}

uint8_t seg_to_lcd(uint8_t seg) {
    if (seg > 16) {
        return 32 - (16 - seg);
    }
    return seg;
}

void CPU::write_vram(uint16_t addr, Nibble value) {
    uint8_t col = ((addr & 0x7F) >> 1);
    uint8_t row = (((addr & 0x80) >> 7) * 8 + (addr & 0x1) * 4);
    if (seg_pos[col] < 32) {
        for (int i = 0; i < 4; i++) {
            screen_[seg_pos[col] + (row + i) * 32] = ((value.Get() >> i) & 0b1) * 0xFF;
        }
    }
}


void CPU::log() {
    auto opcode = get_opcode();
    std::cout << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << get_addr() / 3 << ":";
    std::cout << " " << std::setw(3) << opcode << " - " << std::bitset<12>(opcode);
    std::cout << ", SP = " << std::hex << std::setw(2) << static_cast<int>(SP);
    std::cout << ", NP = " << std::hex << std::setw(1) << static_cast<int>(NPCB) << static_cast<int>(NPCP.Get());
    std::cout << ", X = " << std::hex << std::setw(3) << static_cast<int>(IX);
    std::cout << ", Y = " << std::hex << std::setw(3) << static_cast<int>(IY);
    std::cout << ", A = " << std::hex << std::setw(1) << static_cast<int>(A.Get());
    std::cout << ", B = " << std::hex << std::setw(1) << static_cast<int>(B.Get());
    std::cout << ", F = " << std::hex << std::setw(1) << static_cast<int>(F.Get());
    std::cout << std::endl;
}

void CPU::step() {
    last_opcode_cycles_ = 5;
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
    cycles_ += last_opcode_cycles_;
    process_timers();
    
    if (type != OpcodeType::PSET) {
        if (check_i()) {
            process_interrupts();
        }
        // new page pointer gets reset to current page pointer if not just set
        NPCP = PCP;        
    }

    IX &= 0xFFF;
    IY &= 0xFFF;
}

void CPU::reset() {
    reset_io();
    PCS = 0;
    PCP = 1;
    PCB = false;
    SP = 0;
    NPCP = 1;
    IX = 0;
    IY = 0;
    A = 0;
    B = 0;
    F = 0;
    cycles_ = 0;
    for (int j = 0; j < 6; j++) {
        auto& i = interrupts_[j];
        constexpr std::array<int, 6> vectors = { 0x2, 0x4, 0xC, 0xA, 0x6, 0x8 };
        i.factor = 0;
        i.mask = 0;
        i.dispatched = false;
        i.vector = vectors[j];
    }
    programmable_timer_enabled_ = false;
    programmable_timer_period_ = 0;
    programmable_timer_down_counter_ = 0;
    programmable_timer_down_counter_reload_ = 0;
    programmable_timer_start_ = 0;
}

void CPU::print() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 32; j++) {
            if (screen_[i * 32 + j])
                std::cout << "#";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}