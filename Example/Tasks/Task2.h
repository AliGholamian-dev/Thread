#pragma once


#include "Task.h"

class Task2 : public Task {
    public:
        Task2() = default;
        ~Task2() override = default;
        void doTask() override;
};
