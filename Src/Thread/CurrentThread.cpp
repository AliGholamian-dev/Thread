#include "CurrentThread.h"

namespace ThreadNS::CurrentThread {
    ThreadID ThreadNS::CurrentThread::getID() {
        return std::this_thread::get_id();
    }

    void sleepUntil(const xtime* absoluteTime) {
        std::this_thread::sleep_until(absoluteTime);
    }
}
