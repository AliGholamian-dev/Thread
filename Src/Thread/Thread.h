#pragma once


#include <thread>
#include <mutex>
#include <list>
#include "CurrentThread.h"
#include "ThreadID.h"
#include "TaskID.h"
class Task;


namespace ThreadNS {
    class Thread final {
        public:
            enum class TaskType{
                oneTimeRun,
                continuousRun,
            };

            Thread(Thread& other) = delete;
            Thread(const Thread& other) = delete;
            Thread(Thread&& other) = delete;

            Thread();
            ~Thread();

            bool requestStart();
            bool requestPause();
            bool requestQuit();
            void requestTermination();
            void wait() const volatile;


            [[nodiscard]] bool isRunning() const;
            [[nodiscard]] bool isPaused() const;
            [[nodiscard]] ThreadID getID() const;
            static unsigned int getMaxNumberOfConcurrentThreads();

            TaskID queueTaskForAddition(std::shared_ptr<Task>& task, TaskType taskType);
            void queueTaskForRemoval(const TaskID& taskID);

        private:
            enum class Request {
                start,
                pause,
                quit,
                terminate,
                none
            };

            enum class ThreadState {
                idle,
                running,
                quiting,
                termination,
                terminated
            };

            struct TaskInfo{
                TaskID taskID;
                TaskType taskType;
                std::shared_ptr<Task> task;
            };

            bool checkForRepetitiveID(const TaskID& taskID);
            TaskID generateUniqueTaskID();
            std::list<TaskInfo> copyToBeAddedTasks();
            void addQueuedTasks();
            void accumulateToBeDeletedTasks();
            void clearToBeDeletedTasksList();
            void deletedQueuedTasks();
            void handleRequests();
            void runNextTask();
            void handleStateMachine();
            void provideNextIterator();
            void run();


            bool idCounterOverflowedOnce { false };
            TaskID taskIDCounter { 0 };
            Request request { Request::none };
            ThreadState threadState { ThreadState::idle };
            bool atEnd { false };
            bool wasEmptyBefore { true };
            bool deleteTask { false };
            std::jthread thread;
            std::list<TaskInfo> tasks;
            std::list<TaskInfo> toBeAddedTasks;
            std::list<TaskID> toBeDeletedTasks;
            std::list<TaskID> accumulatedToBeDeletedTasks;
            std::mutex deletionListCopyMutex;
            std::mutex additionListCopyMutex;
            std::list<TaskInfo>::iterator it;
    };
}
