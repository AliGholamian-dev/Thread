#pragma once


#include "Task.h"

class Task1 : public Task {
    public:
        Task1() = default;
        ~Task1() override = default;
        void doTask() override;
};
