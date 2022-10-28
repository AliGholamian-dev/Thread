#include "Task.h"

Task::Task() : aliveCheck(std::make_shared<bool>(true)) {}

Task::~Task() {
    *aliveCheck = false;
}
