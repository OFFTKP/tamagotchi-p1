#ifndef E0C6S46_HXX
#define E0C6S46_HXX
#include "nibble.hxx"
#include "opcodes.hxx"
#include <vector>
#include <string>
#include <array>

constexpr static inline uint16_t CpuFrequency = 32768;

class Tama;

enum InterruptType {
    Clock,
    Stopwatch,
    Programmable,
    Serial,
    K00K03,
    K10K13,
    InterruptTypeSize
};

struct Interrupt {
    Nibble factor;
    Nibble mask;
    bool dispatched;
    uint16_t vector;
};

class CPU {
public:
    void LoadRom(std::string filename);
    void Step();
    const std::array<uint8_t, 32 * 16>& GetDisplay();
    void KeyPress(int key);
    void KeyRelease(int key);
private:
    uint16_t get_opcode();
    uint16_t get_addr();
    bool check_c(), check_z(), check_d(), check_i();
    void set_c(bool), set_z(bool), set_d(bool), set_i(bool);
    Nibble read(uint16_t addr);
    Nibble read_ram(uint16_t addr);
    void write_ram(uint16_t addr, Nibble data);
    Nibble read_vram(uint16_t addr);
    void write_vram(uint16_t addr, Nibble data);
    Nibble read_io(uint16_t addr);
    void write_io(uint16_t addr, Nibble data);
    void reset_io();
    Nibble read_rq(uint8_t rq);
    void write_rq(uint8_t rq, Nibble data);
    void log();
    void reset();

    Nibble A { 0 }, B { 0 };
    Nibble F { 0 };
    uint8_t SP { 0 };
    uint16_t IX { 0 }, IY { 0 };
    
    uint8_t PCS { 0 }; // Step (0-0xFF)
    Nibble  PCP { 1 }, NPCP { 1 }; // Page (0-0xF)
    bool    PCB { false }, NPCB { false }; // Bank (0-1)

    std::vector<Nibble> memory_;
    std::array<Nibble, 640> ram_ {};
    std::array<Nibble, 0x80> io_ {};
    std::array<Nibble, 160> vram_ {};
    std::array<Interrupt, InterruptTypeSize> interrupts_ {};
    std::array<uint8_t, 32 * 16> screen_ {};
    uint16_t opcode_;
    uint32_t cycles_ { 0 };
    uint8_t last_opcode_cycles_ { 0 };
    uint32_t programmable_timer_start_ { 0 };
    uint16_t programmable_timer_down_counter_ { 0 };
    uint16_t programmable_timer_down_counter_reload_ { 0 };
    uint8_t programmable_timer_period_ { 0 };
    bool programmable_timer_enabled_ { false };
    std::array<bool, 3> keys_pressed_ { false, false, false };

    void step();
    void print();
    OpcodeType decode(uint16_t);
    void process_timers();
    void dispatch_interrupt(InterruptType, int);
    void process_interrupts();

    #define XMACRO(name, unused, ...) void _##name();
    #include "opcodes.def"
    #undef XMACRO
};
#endif