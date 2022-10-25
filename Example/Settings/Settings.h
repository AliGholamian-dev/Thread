#pragma once

#include "FlagClass.h"

namespace Settings {

    namespace Debug {
        struct Setting final {
            FlagClass DebugMode {
                    enum Value {
                        inConsole = 0b01,
                        inFile = 0b10
                    };
                    Flag_Class(DebugMode)
            };
            Setting();
            const bool showDebugMessages;
            const DebugMode debugModes;
        } extern setting;
    }

    namespace Exception {
        struct Setting final {
            Setting();
            const bool showExceptionMessage;
        } extern setting;
    }

    namespace Example {
        struct Setting final {
            Setting();
            const bool showTask1DebugMessage;
            const bool showTask2DebugMessage;
            const bool showTask3DebugMessage;
            const bool showMainDebugMessage;
        } extern setting;
    }
}
