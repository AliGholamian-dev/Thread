#include "Thread.h"
#include "Task.h"


namespace ThreadNS {
    Thread::Thread() :
            thread(&Thread::run, this),
            it(tasks.begin()) {}

    Thread::~Thread() {
        threadState = ThreadState::terminated;
        thread.detach();
    }

    void Thread::requestStart() {
        if (!isStarted()) {
            std::lock_guard<std::mutex> guard(requestMutex);
            request = Request::start;
        }
    }

    void Thread::requestResume() {
        if (request != Request::terminate && isPaused()) {
            std::lock_guard<std::mutex> guard(requestMutex);
            request = Request::resume;
        }
    }

    void Thread::requestPause() {
        if (request != Request::terminate && isRunning()) {
            std::lock_guard<std::mutex> guard(requestMutex);
            request = Request::pause;
        }
    }

    void Thread::requestQuitOnEndOfTaskLoop() {
        if (request != Request::terminate && (isRunning() || isPaused())) {
            std::lock_guard<std::mutex> guard(requestMutex);
            request = Request::quit;
        }
    }

    void Thread::requestTermination() {
        if (isStarted() && !isFinished()) {
            std::lock_guard<std::mutex> guard(requestMutex);
            request = Request::terminate;
        }
    }

    void Thread::wait() const volatile {
        while (!isFinished() && isStarted());
    }

    bool Thread::isStarted() const volatile {
        return threadState != ThreadState::notStarted;
    }

    bool Thread::isRunning() const {
        return threadState == ThreadState::running;
    }

    bool Thread::isPaused() const {
        return threadState == ThreadState::idle;
    }

    bool Thread::isFinished() const volatile {
        return threadState == ThreadState::terminated;
    }

    ThreadID Thread::getID() const {
        return thread.get_id();
    }

    unsigned int Thread::getMaxNumberOfConcurrentThreads() {
        return std::thread::hardware_concurrency();
    }

    bool Thread::checkForRepetitiveID(const TaskID& taskID) {
        if (taskID == 0) {
            return true;
        }
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

    std::optional<TaskID> Thread::queueTaskForAddition(const std::shared_ptr<Task>& task, TaskType taskType) {
        std::optional<TaskID> taskID;

        if (!isFinished()) {
            if (task && task->aliveCheck && *task->aliveCheck) {
                taskID = generateUniqueTaskID();

                TaskInfo taskInfo;
                taskInfo.taskID = *taskID;
                taskInfo.taskType = taskType;
                taskInfo.taskSharedPointer = task;
                taskInfo.taskRawPointer = nullptr;
                taskInfo.aliveCheck = task->aliveCheck;

                additionListCopyMutex.lock();
                toBeAddedTasks.push_back(std::move(taskInfo));
                additionListCopyMutex.unlock();
            }
        }

        return taskID;
    }

    std::optional<TaskID> Thread::queueTaskForAddition(Task* task, Thread::TaskType taskType) {
        std::optional<TaskID> taskID;

        if(!isFinished()) {
            if(task && task->aliveCheck && *task->aliveCheck) {
                taskID = generateUniqueTaskID();

                TaskInfo taskInfo;
                taskInfo.taskID = *taskID;
                taskInfo.taskType = taskType;
                taskInfo.taskSharedPointer = nullptr;
                taskInfo.taskRawPointer = task;
                taskInfo.aliveCheck = task->aliveCheck;

                additionListCopyMutex.lock();
                toBeAddedTasks.push_back(std::move(taskInfo));
                additionListCopyMutex.unlock();
            }
        }

        return taskID;
    }

    void Thread::queueTaskForRemoval(const TaskID& taskID) {
        if(!isFinished()) {
            std::lock_guard<std::mutex> guard(deletionListCopyMutex);
            toBeDeletedTasks.push_back(taskID);
        }
    }

    std::list<Thread::TaskInfo> Thread::copyToBeAddedTasks() {
        std::lock_guard<std::mutex> guard(additionListCopyMutex);
        auto toBeAddedTasksCopy = toBeAddedTasks;
        toBeAddedTasks.clear();
        return toBeAddedTasksCopy;
    }

    void Thread::addQueuedTasks() {
        auto toBeAddedTasksCopy = copyToBeAddedTasks();
        if (tasks.empty()) {
            tasks = std::move(toBeAddedTasksCopy);
        } else {
            tasks.splice(tasks.end(), toBeAddedTasksCopy);
        }
    }

    void Thread::accumulateToBeDeletedTasks() {
        std::lock_guard<std::mutex> guard(deletionListCopyMutex);
        if (accumulatedToBeDeletedTasks.empty()) {
            accumulatedToBeDeletedTasks = std::move(toBeDeletedTasks);
        } else {
            accumulatedToBeDeletedTasks.splice(accumulatedToBeDeletedTasks.end(), toBeDeletedTasks);
        }
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
            auto foundIt = std::find(accumulatedToBeDeletedTasks.begin(), accumulatedToBeDeletedTasks.end(), taskInfo.taskID);
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
                threadState = ThreadState::running;
                quitOnEndOfTaskLoop = true;
                break;
            }
            case Request::terminate: {
                threadState = ThreadState::terminated;
                break;
            }
            default:
                break;
        }
        request = Request::none;
    }

    void Thread::runNextTask() {
        if (!tasks.empty()) {
            deleteTask = true;
            auto& taskInfo = *it;
            auto lockedTask = taskInfo.aliveCheck.lock();

            if(!taskInfo.aliveCheck.expired()) {
                if(taskInfo.taskSharedPointer) {
                    taskInfo.taskSharedPointer->doTask();
                }
                else if(taskInfo.taskRawPointer) {
                    taskInfo.taskRawPointer->doTask();
                }

                if (taskInfo.taskType == TaskType::continuousRun) {
                    deleteTask = false;
                }
            }
        }
    }

    void Thread::handleStateMachine() {
        if (atEnd && quitOnEndOfTaskLoop) {
            threadState = ThreadState::terminated;
        }

        switch (threadState) {
            case ThreadState::running: {
                runNextTask();
                break;
            }
            case ThreadState::terminated: {
                clearLists();
                break;
            }
            default:
                break;
        }
    }

    void Thread::provideNextIterator() {
        if (tasks.empty()) {
            wasEmptyBefore = true;
            atEnd = true;
        }
        else {
            if (wasEmptyBefore) {
                it = tasks.begin();
                wasEmptyBefore = false;
            }
            else {
                if (deleteTask) {
                    it = tasks.erase(it);
                    deleteTask = false;
                    if (tasks.empty()) {
                        wasEmptyBefore = true;
                        atEnd = true;
                        return;
                    }
                }
                else {
                    if(isRunning()) {
                        it++;
                    }
                }
            }

            if (it == tasks.end()) {
                atEnd = true;
                it = tasks.begin();
            }
            else {
                atEnd = false;
            }
        }
    }

    void Thread::clearLists() {
        tasks.clear();
        toBeAddedTasks.clear();
        toBeDeletedTasks.clear();
        accumulatedToBeDeletedTasks.clear();
    }

    void Thread::run() {
        while (!isFinished()) {
            addQueuedTasks();
            provideNextIterator();
            deletedQueuedTasks();
            handleRequests();
            handleStateMachine();
            clearToBeDeletedTasksList();
        }
    }
}
