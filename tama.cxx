#include "tama.hxx"

void Tama::LoadRom(std::string filename) {
    cpu_.LoadRom(filename);
}

void Tama::Step() {
    cpu_.Step();
}

const std::array<uint8_t, 32 * 16>& Tama::GetDisplay() {
    return cpu_.GetDisplay();
}

void Tama::KeyPress(int key) {
    cpu_.KeyPress(key);
}

void Tama::KeyRelease(int key) {
    cpu_.KeyRelease(key);
}