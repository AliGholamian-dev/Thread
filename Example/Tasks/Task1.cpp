#include "Task1.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "CurrentThread.h"


static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showTask1DebugMessage);
    debug << "[Task1]: ";
    return debug;
}

void Task1::doTask() {
    debug() << "Thread(#"  << ThreadNS::CurrentThread::getID() << ")"<< std::endl;
    ThreadNS::CurrentThread::sleepFor(std::chrono::seconds(1));
    debug() << "Thread(#"  << ThreadNS::CurrentThread::getID() << ") Sleep Finished"<< std::endl;
}
