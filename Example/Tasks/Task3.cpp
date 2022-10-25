#include "Task3.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "Thread.h"
#include <iomanip>


static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showTask3DebugMessage);
    debug.noSpace();
    debug << "[Task3]: Thread(#"  << std::setfill('0') << std::setw(10) << ThreadNS::CurrentThread::getID() << ")";
    return debug;
}

Task3::Task3(const std::vector<std::shared_ptr<ThreadNS::Thread>>& threads) : threads(threads) {}

void Task3::doTask() {
    ThreadNS::CurrentThread::sleepFor(std::chrono::seconds(30));
    debug() << " -> Terminating all threads" << std::endl;
    for(auto& thread : threads) {
        thread->requestTermination();
    }
}
