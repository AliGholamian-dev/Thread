#include "Task1.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "CurrentThread.h"
#include <iomanip>


static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showTask1DebugMessage);
    debug.noSpace();
    debug << "[Task1]: Thread(#"  << std::setfill('0') << std::setw(10) << ThreadNS::CurrentThread::getID() << ")";
    return debug;
}

void Task1::doTask() {
    debug() << std::endl;
    ThreadNS::CurrentThread::sleepFor(std::chrono::seconds(1));
    debug() << " -> Sleep Finished" << std::endl;
}
