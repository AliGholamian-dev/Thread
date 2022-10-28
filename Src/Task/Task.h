#pragma once


#include <memory>
#include "Thread.h"

class Task {
    public:
        Task();
        virtual ~Task();
        virtual void doTask() = 0;

    private:
        friend class ThreadNS::Thread;
        std::shared_ptr<bool> aliveCheck;
};
