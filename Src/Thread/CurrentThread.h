#pragma once
#include <thread>
#include "ThreadID.h"

namespace ThreadNS::CurrentThread {
    ThreadID getID();

    template <class Clock, class Duration>
    void sleepUntil(const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        std::this_thread::sleep_until(absoluteTime);
    }

    template <class Rep, class Period>
    void sleepFor(const std::chrono::duration<Rep, Period>& relativeTime) {
        std::this_thread::sleep_for(relativeTime);
    }
}
