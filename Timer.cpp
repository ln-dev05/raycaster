#include "Timer.hpp"
#include "quickcg.h"
Timer::Timer() {
    old_time = 0;
    time = 0;
    frame_time = 0;
}

void Timer::update() {
    old_time = time;
    time = QuickCG::getTicks();
    frame_time = (time - old_time) / 1000.0;
}