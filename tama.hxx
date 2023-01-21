#ifndef TAMA_HXX
#define TAMA_HXX

#include "cpu.hxx"

class Tama {
public:
    void LoadRom(std::string filename);
private:
    CPU cpu_;
};

#endif