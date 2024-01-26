#include "Timer.hpp"

Timer::Timer() {
    old_time = 0;
    time = 0;
    frame_time = 0;
    frame_rate = 0;
}

void Timer::update(unsigned long tick) {
    old_time = time;
    time = tick;
    frame_time = (time - old_time) / 1000.0;
    frame_rate = 1.0/frame_time;
}
