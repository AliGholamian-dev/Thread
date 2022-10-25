#pragma once

#include <cstdint>

#define FlagClass   struct

#define Flag_Class(ClassName) \
    public: \
        ClassName() = delete; \
        inline explicit ClassName(uint64_t flags) : flags(flags) {} \
        inline bool isFlagSet(const Value& toBeCheckedFlag) const {return flags & toBeCheckedFlag;} \
    private: \
        uint64_t flags; \
    public:
