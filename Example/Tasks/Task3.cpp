#include "Task3.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "Thread.h"

static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showTask3DebugMessage);
    debug << "[Task3]: ";
    return debug;
}

Task3::Task3(const std::vector<std::shared_ptr<ThreadNS::Thread>>& threads) : threads(threads) {}

void Task3::doTask() {
    ThreadNS::CurrentThread::sleepFor(std::chrono::seconds(10));
    debug() << "Thread(#"  << ThreadNS::CurrentThread::getID() << ") -> Terminating all threads"<< std::endl;
    for(auto& thread : threads) {
        thread->requestTermination();
    }
}
