#include "cpu.hxx"
#include <iostream>

void CPU::process_timers() {
    if (programmable_timer_enabled_) {
        if (cycles_ - programmable_timer_start_ >= programmable_timer_period_) {
            do {
                programmable_timer_start_ += programmable_timer_period_;
                programmable_timer_down_counter_--;
                if (programmable_timer_down_counter_ == 0) {
                    dispatch_interrupt(Programmable, 0);
                    programmable_timer_down_counter_ = programmable_timer_down_counter_reload_;
                }
            } while (cycles_ - programmable_timer_start_ >= programmable_timer_period_);
        }
    }
}

void CPU::process_interrupts() {
    if (check_i()) {
        for (int i = 0; i < InterruptTypeSize; ++i) {
            if (interrupts_[i].dispatched) {
                set_i(false);
                write_ram(--SP, PCP);
                write_ram(--SP, PCS >> 4);  
                write_ram(--SP, PCS & 0xF);
                PCB = 0;
                PCP = 1;
                PCS = interrupts_[i].vector;
                // Time equivalent to 12 cycles of CPU system clock is required to execute the interrupt
                cycles_ += 12;
                interrupts_[i].dispatched = false;
            }
        }
    }
}

void CPU::dispatch_interrupt(InterruptType interrupt, int bit) {
    interrupts_[interrupt].factor |= 1 << bit;
    if (interrupts_[interrupt].mask.Get() & (1 << bit)) {
        interrupts_[interrupt].dispatched = true;
    } else {
        std::cout << "couldnt run interrupt cus mask " << static_cast<int>(interrupts_[interrupt].mask.Get()) << std::endl;
    }
}