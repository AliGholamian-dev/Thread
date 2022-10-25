#include "Thread.h"
#include "Task1.h"
#include "Task2.h"
#include "Task3.h"
#include "Task.h"


int main() {
    std::shared_ptr<ThreadNS::Thread> thread1 = std::make_shared<ThreadNS::Thread>();
    std::shared_ptr<ThreadNS::Thread> thread2 = std::make_shared<ThreadNS::Thread>();
    std::shared_ptr<ThreadNS::Thread> thread3 = std::make_shared<ThreadNS::Thread>();
    std::vector<std::shared_ptr<ThreadNS::Thread>> threads({thread1, thread2, thread3});
    std::shared_ptr<Task> task1 = std::make_shared<Task1>();
    std::shared_ptr<Task> task2 = std::make_shared<Task2>();
    std::shared_ptr<Task> task3 = std::make_shared<Task3>(threads);

    thread1->queueTaskForAddition(task2, ThreadNS::Thread::TaskType::continuousRun);
    thread2->queueTaskForAddition(task1, ThreadNS::Thread::TaskType::continuousRun);
    thread1->requestStart();
    thread2->requestStart();
    thread1->queueTaskForAddition(task1, ThreadNS::Thread::TaskType::oneTimeRun);
    thread2->queueTaskForAddition(task2, ThreadNS::Thread::TaskType::oneTimeRun);
    thread3->queueTaskForAddition(task3, ThreadNS::Thread::TaskType::oneTimeRun);
    thread3->requestStart();
    thread1->wait();
    thread2->wait();
    return 0;
}
