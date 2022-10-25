#pragma once


#include "Task.h"
#include <vector>
#include <memory>
namespace ThreadNS {
    class Thread;
}

class Task3 : public Task {
    public:
        explicit Task3(const std::vector<std::shared_ptr<ThreadNS::Thread>>& threads);
        ~Task3() override = default;
        void doTask() override;

    private:
        std::vector<std::shared_ptr<ThreadNS::Thread>> threads;
};
