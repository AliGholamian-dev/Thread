#pragma once


#include <thread>
#include <mutex>
#include <list>
#include <optional>
#include <memory>
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

            void requestStart();
            void requestResume();
            void requestPause();
            void requestQuitOnEndOfTaskLoop();
            void requestTermination();
            void wait() const volatile;


            [[nodiscard]] bool isStarted() const volatile;
            [[nodiscard]] bool isRunning() const;
            [[nodiscard]] bool isPaused() const;
            [[nodiscard]] bool isFinished() const volatile;
            [[nodiscard]] ThreadID getID() const;
            static unsigned int getMaxNumberOfConcurrentThreads();

            std::optional<TaskID> queueTaskForAddition(const std::shared_ptr<Task>& task, TaskType taskType);
            std::optional<TaskID> queueTaskForAddition(Task* task, TaskType taskType);
            void queueTaskForRemoval(const TaskID& taskID);

        private:
            enum class Request {
                start,
                resume,
                pause,
                quit,
                terminate,
                none
            };

            enum class ThreadState {
                notStarted,
                idle,
                running,
                terminated
            };

            struct TaskInfo{
                TaskID taskID;
                TaskType taskType;
                std::shared_ptr<Task> taskSharedPointer;
                Task* taskRawPointer;
                std::weak_ptr<bool> aliveCheck;
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
            void clearLists();


            ThreadState threadState { ThreadState::notStarted };
            Request request { Request::none };
            TaskID taskIDCounter { 0 };
            bool idCounterOverflowedOnce { false };
            bool atEnd { false };
            bool wasEmptyBefore { true };
            bool deleteTask { false };
            bool quitOnEndOfTaskLoop { false };

            std::jthread thread;
            std::list<TaskInfo> tasks;
            std::list<TaskInfo> toBeAddedTasks;
            std::list<TaskID> toBeDeletedTasks;
            std::list<TaskID> accumulatedToBeDeletedTasks;
            std::mutex requestMutex;
            std::mutex deletionListCopyMutex;
            std::mutex additionListCopyMutex;
            std::list<TaskInfo>::iterator it;
    };
}
