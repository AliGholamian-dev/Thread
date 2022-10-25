#include "Task2.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "CurrentThread.h"


static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showTask2DebugMessage);
    debug << "[Task2]: ";
    return debug;
}

void Task2::doTask() {
    debug() << "Thread(#"  << ThreadNS::CurrentThread::getID() << ")"<< std::endl;
    ThreadNS::CurrentThread::sleepFor(std::chrono::seconds(2));
    debug() << "Thread(#"  << ThreadNS::CurrentThread::getID() << ") Sleep Finished"<< std::endl;
}