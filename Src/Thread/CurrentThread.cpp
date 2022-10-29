#include "CurrentThread.h"

namespace ThreadNS::CurrentThread {
        ThreadID getID() {
            return std::this_thread::get_id();
        }
    }
