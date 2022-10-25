#include "Task2.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "CurrentThread.h"
#include <iomanip>


static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showTask2DebugMessage);
    debug.noSpace();
    debug << "[Task2]: Thread(#"  << std::setfill('0') << std::setw(10) << ThreadNS::CurrentThread::getID() << ")";
    return debug;
}

void Task2::doTask() {
    debug() << std::endl;
    ThreadNS::CurrentThread::sleepFor(std::chrono::seconds(2));
    debug() << " -> Sleep Finished" << std::endl;
}