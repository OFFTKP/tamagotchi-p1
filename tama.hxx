#ifndef TAMA_HXX
#define TAMA_HXX

#include "cpu.hxx"

class Tama {
public:
    void LoadRom(std::string filename);
    void Step();
    const std::array<uint8_t, 32 * 16>& GetDisplay();
    void KeyPress(int);
    void KeyRelease(int);
private:
    CPU cpu_;
};

#endif