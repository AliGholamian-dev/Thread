#include "Thread.h"
#include "Task.h"

namespace ThreadNS {
    Thread::Thread() :
            thread(&Thread::run, this),
            it(tasks.begin()) {}

    Thread::~Thread() {
        requestTermination();
        thread.detach();
    }

    bool Thread::requestStart() {
        if (request == Request::terminate ||
            request == Request::quit ||
            threadState == ThreadState::quiting ||
            threadState == ThreadState::termination) {
            return false;
        }
        request = Request::start;
        return true;
    }

    bool Thread::requestPause() {
        if (request == Request::terminate ||
            request == Request::quit ||
            threadState == ThreadState::quiting ||
            threadState == ThreadState::termination) {
            return false;
        }
        request = Request::pause;
        return true;
    }

    bool Thread::requestQuit() {
        if (request == Request::terminate ||
            threadState == ThreadState::termination) {
            return false;
        }
        request = Request::quit;
        return true;
    }


    void Thread::requestTermination() {
        request = Request::terminate;
    }

    void Thread::wait() const volatile {
        while (threadState != ThreadState::terminated);
    }

    bool Thread::isRunning() const {
        return threadState == ThreadState::running;
    }

    bool Thread::isPaused() const {
        return threadState == ThreadState::idle;
    }

    ThreadID Thread::getID() const {
        return thread.get_id();
    }

    unsigned int Thread::getMaxNumberOfConcurrentThreads() {
        return std::thread::hardware_concurrency();
    }

    bool Thread::checkForRepetitiveID(const TaskID& taskID) {
        if (taskID == 0)
            return true;
        auto foundIt = std::find_if(tasks.begin(), tasks.end(), [&taskID](const TaskInfo& taskIDPair) -> bool {
            if (taskIDPair.taskID == taskID) {
                return true;
            }
            return false;
        });
        if (foundIt != tasks.end()) {
            return true;
        }
        return false;
    }

    TaskID Thread::generateUniqueTaskID() {
        if (idCounterOverflowedOnce) {
            while (checkForRepetitiveID(taskIDCounter)) {
                taskIDCounter++;
            }
        } else {
            taskIDCounter++;
            if (taskIDCounter == std::numeric_limits<unsigned long>::max()) {
                idCounterOverflowedOnce = true;
                taskIDCounter = 1;
            }
        }
        return taskIDCounter;
    }

    TaskID Thread::queueTaskForAddition(std::shared_ptr<Task>& task, TaskType taskType) {
        std::lock_guard<std::mutex> guard(additionListCopyMutex);

        TaskID taskID = generateUniqueTaskID();
        TaskInfo taskInfo;
        taskInfo.taskID = taskID;
        taskInfo.taskType = taskType;
        taskInfo.task = task;
        toBeAddedTasks.push_back(std::move(taskInfo));

        return taskID;
    }

    void Thread::queueTaskForRemoval(const TaskID& taskID) {
        std::lock_guard<std::mutex> guard(deletionListCopyMutex);
        toBeDeletedTasks.push_back(taskID);
    }

    std::list<Thread::TaskInfo> Thread::copyToBeAddedTasks() {
        additionListCopyMutex.lock();
        auto toBeAddedTasksCopy = toBeAddedTasks;
        toBeAddedTasks.clear();
        additionListCopyMutex.unlock();
        return toBeAddedTasksCopy;
    }

    void Thread::addQueuedTasks() {
        auto toBeAddedTasksCopy = copyToBeAddedTasks();
        if (tasks.empty()) {
            tasks = toBeAddedTasksCopy;
        } else {
            tasks.splice(tasks.end(), toBeAddedTasksCopy);
        }
    }

    void Thread::accumulateToBeDeletedTasks() {
        deletionListCopyMutex.lock();
        if (accumulatedToBeDeletedTasks.empty()) {
            accumulatedToBeDeletedTasks = toBeDeletedTasks;
        } else {
            accumulatedToBeDeletedTasks.insert(accumulatedToBeDeletedTasks.end(),
                                               toBeDeletedTasks.begin(),
                                               toBeDeletedTasks.end());
        }
        toBeDeletedTasks.clear();
        deletionListCopyMutex.unlock();
    }

    void Thread::clearToBeDeletedTasksList() {
        if (atEnd) {
            accumulatedToBeDeletedTasks.clear();
        }
    }

    void Thread::deletedQueuedTasks() {
        accumulateToBeDeletedTasks();
        if (!tasks.empty()) {
            auto& taskInfo = *it;
            auto foundIt =
                    std::find(accumulatedToBeDeletedTasks.begin(), accumulatedToBeDeletedTasks.end(), taskInfo.taskID);
            if (foundIt != accumulatedToBeDeletedTasks.end()) {
                accumulatedToBeDeletedTasks.erase(foundIt);
                deleteTask = true;
            }
        }
    }

    void Thread::handleRequests() {
        switch (request) {
            case Request::start: {
                threadState = ThreadState::running;
                break;
            }
            case Request::pause: {
                threadState = ThreadState::idle;
                break;
            }
            case Request::quit: {
                threadState = ThreadState::quiting;
                break;
            }
            case Request::terminate: {
                threadState = ThreadState::termination;
                break;
            }
            case Request::none:
            default:
                break;
        }
        request = Request::none;
    }

    void Thread::runNextTask() {
        if (!tasks.empty()) {
            auto& taskInfo = *it;
            taskInfo.task->doTask();
            if (taskInfo.taskType == TaskType::oneTimeRun) {
                deleteTask = true;
            }
        }
    }

    void Thread::handleStateMachine() {
        switch (threadState) {
            case ThreadState::running: {
                runNextTask();
                break;
            }
            case ThreadState::quiting: {
                if (atEnd) {
                    threadState = ThreadState::terminated;
                } else {
                    runNextTask();
                }
                break;
            }
            case ThreadState::termination: {
                threadState = ThreadState::terminated;
                break;
            }
            case ThreadState::terminated:
            case ThreadState::idle:
            default:
                break;
        }
    }

    void Thread::provideNextIterator() {
        if (tasks.empty()) {
            wasEmptyBefore = true;
            atEnd = true;
        } else {
            if (wasEmptyBefore) {
                it = tasks.begin();
                wasEmptyBefore = false;
            } else {
                if (deleteTask) {
                    it = tasks.erase(it);
                    deleteTask = false;
                } else {
                    it++;
                }
            }
            if (it != tasks.end()) {
                atEnd = false;
            } else {
                atEnd = true;
                it = tasks.begin();
            }
        }
    }

    void Thread::run() {
        while (threadState != ThreadState::terminated) {
            addQueuedTasks();
            provideNextIterator();
            deletedQueuedTasks();
            handleRequests();
            handleStateMachine();
            clearToBeDeletedTasksList();
        }
    }
}
