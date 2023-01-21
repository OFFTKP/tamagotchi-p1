#ifndef E0C6S46_HXX
#define E0C6S46_HXX
#include "nibble.hxx"
#include "opcodes.hxx"
#include <vector>
#include <string>
#include <array>

class Tama;

class CPU {
public:
    void LoadRom(std::string filename);
private:
    uint16_t get_ix(), get_iy();
    uint8_t get_x(), get_y();
    Nibble get_xh(), get_yh();
    Nibble get_xl(), get_yl();
    Nibble get_xp(), get_yp();
    uint8_t get_sp();
    Nibble get_sph(), get_spl();
    Nibble get_mx(), get_my(); // TODO: remove
    uint16_t get_opcode();
    uint16_t get_addr();
    bool check_c(), check_z(), check_d(), check_i();
    void set_c(bool), set_z(bool), set_d(bool), set_i(bool);
    Nibble read(uint16_t addr);
    Nibble read_ram(uint16_t addr);
    void write_ram(uint16_t addr, Nibble data);
    Nibble read_io(uint16_t addr);
    void write_io(uint16_t addr, Nibble data);
    Nibble read_rq(uint8_t rq);
    void write_rq(uint8_t rq, Nibble data);
    void log();
    void reset();

    Nibble A, B;
    Nibble F;
    uint8_t SP;
    uint16_t IX, IY;
    
    uint8_t PCS { 0 }; // Step (0-0xFF)
    Nibble  PCP { 1 }, NPCP { 1 }; // Page (0-0xF)
    bool    PCB { false }, NPCB { false }; // Bank (0-1)

    std::vector<Nibble> memory_;
    std::array<Nibble, 640> ram_ {};
    uint16_t opcode_;

    void step();
    OpcodeType decode(uint16_t);

    #define XMACRO(name, unused, ...) void _##name();
    #include "opcodes.def"
    #undef XMACRO
};
#endif