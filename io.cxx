#include "cpu.hxx"
#include <iostream>
#include <iomanip>

constexpr static inline uint16_t hz_to_period(uint16_t hz) {
    return CpuFrequency / hz;
}

void CPU::reset_io() {
    io_[0x40] = 0b1111; /* Input port K00-K03, K00-K02 is responsible for the buttons */
    io_[0x70] = 0b0000; /* OSC frequency change, unused */
    io_[0x73] = 0b1000; /* SVDON */
}

Nibble CPU::read_io(uint16_t addr) {
    #define writeonly return 0
    auto data = io_[addr & 0x7F].Get();
    switch (addr) {
        case 0xF00:
        case 0xF01:
        case 0xF02:
        case 0xF03:
        case 0xF04:
        case 0xF05: {
            auto ret = interrupts_[addr - 0xF00].factor;
            interrupts_[addr - 0xF00].factor = 0;
            return ret;
        }
        case 0xF10:
        case 0xF11:
        case 0xF12:
        case 0xF13:
        case 0xF14:
        case 0xF15: return interrupts_[addr - 0xF10].mask;
        case 0xF24: return programmable_timer_down_counter_ & 0xF;
        case 0xF25: return programmable_timer_down_counter_ >> 4;
        case 0xF26: return programmable_timer_down_counter_reload_ & 0xF;
        case 0xF27: return programmable_timer_down_counter_reload_ >> 4;
        case 0xF40: {
            return 0b1000 | (!keys_pressed_[2] << 2) | (!keys_pressed_[1] << 1) | (!keys_pressed_[0] << 0);
        }
        case 0xF70: break;
        case 0xF73: {
            if (data & 0b0100) {
                // Return ok voltage
                data = 0b0110;
            }
            break;
        }
        case 0xF78: return programmable_timer_enabled_;
        default:
            // std::cout << "Unimplemented io read from: " << std::hex << std::setfill('0') << std::setw(3) << addr << std::endl;
            break;
    }
    return data;
    #undef writeonly
}

void CPU::write_io(uint16_t addr, Nibble value) {
    #define readonly return
    auto data = value.Get();
    switch (addr) {
        case 0xF00:
        case 0xF01:
        case 0xF02:
        case 0xF03:
        case 0xF04:
        case 0xF05: readonly;
        case 0xF10:
        case 0xF11:
        case 0xF12:
        case 0xF13:
        case 0xF14:
        case 0xF15: {
            interrupts_[addr - 0xF10].mask = data;
            return;
        }
        case 0xF24:
        case 0xF25: readonly;
        case 0xF26: {
            programmable_timer_down_counter_reload_ = (programmable_timer_down_counter_reload_ & 0xF0) | data;
            return;
        }
        case 0xF27: {
            programmable_timer_down_counter_reload_ = (programmable_timer_down_counter_reload_ & 0x0F) | (data << 4);
            return;
        }
        case 0xF40: readonly;
        case 0xF70: break;
        case 0xF73: {
            data &= 0b111;
            break;
        }
        case 0xF78: {
            if (data & 0b10) {
                programmable_timer_down_counter_ = programmable_timer_down_counter_reload_;
                if (programmable_timer_down_counter_ == 0) {
                    // 8.3.4 When the reload register (RD0–RD7) value is set at "00H", the down-counter becomes a 256-value counter.
                    programmable_timer_down_counter_ = 256;
                }
            }
            bool was_enabled = programmable_timer_enabled_;
            programmable_timer_enabled_ = data & 0b1;
            if (!was_enabled && programmable_timer_enabled_) {
                programmable_timer_start_ = cycles_;
            }
            break;
        }
        case 0xF79: {
            switch (data & 0b111) {
                case 0b000: break;
                case 0b001: break;
                case 0b010: {
                    programmable_timer_period_ = hz_to_period(256);
                    break;
                }
                case 0b011: {
                    programmable_timer_period_ = hz_to_period(512);
                    break;
                }
                case 0b100: {
                    programmable_timer_period_ = hz_to_period(1024);
                    break;
                }
                case 0b101: {
                    programmable_timer_period_ = hz_to_period(2048);
                    break;
                }
                case 0b110: {
                    programmable_timer_period_ = hz_to_period(4096);
                    break;
                }
                case 0b111: {
                    programmable_timer_period_ = hz_to_period(8192);
                    break;
                }
            }
            break;
        }
        default:
            // std::cout << "Unimplemented io write to: " << std::hex << std::setfill('0') << std::setw(3) << addr << std::endl;
            break;
    }
    io_[addr & 0xFF] = data;
    #undef readonly
}