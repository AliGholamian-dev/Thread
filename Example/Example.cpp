#include "Thread.h"
#include "Settings.h"
#include "ConditionalDebug.h"
#include "Task1.h"
#include "Task2.h"
#include "Task3.h"
#include "Task.h"
#include <iomanip>


static inline ConditionalDebug debug() {
    ConditionalDebug debug(Settings::Example::setting.showMainDebugMessage);
    debug.noSpace();
    debug << "[ Main]: Thread(#"  << std::setfill('0') << std::setw(10) << ThreadNS::CurrentThread::getID() << ")";
    return debug;
}

int main() {

    debug() << " -> Creating threads" << std::endl;
    std::shared_ptr<ThreadNS::Thread> thread1 = std::make_shared<ThreadNS::Thread>();
    std::shared_ptr<ThreadNS::Thread> thread2 = std::make_shared<ThreadNS::Thread>();
    std::shared_ptr<ThreadNS::Thread> thread3 = std::make_shared<ThreadNS::Thread>();
    std::vector<std::shared_ptr<ThreadNS::Thread>> threads({thread1, thread2, thread3});

    debug() << " -> Creating tasks" << std::endl;
    std::shared_ptr<Task> task1 = std::make_shared<Task1>();
    std::shared_ptr<Task> task2 = std::make_shared<Task2>();
    std::shared_ptr<Task> task3 = std::make_shared<Task3>(threads);

    debug() << " -> Adding tasks to first thread" << std::endl;
    thread1->queueTaskForAddition(task1, ThreadNS::Thread::TaskType::oneTimeRun);
    thread1->queueTaskForAddition(task1, ThreadNS::Thread::TaskType::continuousRun);
    thread1->queueTaskForAddition(task2, ThreadNS::Thread::TaskType::continuousRun);

    debug() << " -> Starting first thread" << std::endl;
    thread1->requestStart();

    debug() << " -> Starting second thread" << std::endl;
    thread2->requestStart();

    debug() << " -> Adding tasks to second thread" << std::endl;
    thread2->queueTaskForAddition(task2, ThreadNS::Thread::TaskType::oneTimeRun);
    thread2->queueTaskForAddition(task2, ThreadNS::Thread::TaskType::continuousRun);
    thread2->queueTaskForAddition(task1, ThreadNS::Thread::TaskType::continuousRun);

    debug() << " -> Adding tasks to third thread" << std::endl;
    thread3->queueTaskForAddition(task3, ThreadNS::Thread::TaskType::oneTimeRun);

    debug() << " -> Starting third thread" << std::endl;
    thread3->requestStart();

    debug() << " -> Waiting for threads to finish" << std::endl;
    thread1->wait();
    thread2->wait();

    debug() << " -> Finished" << std::endl;
    return 0;
}
