#include "Settings.h"

namespace Settings {
    namespace Debug      {Setting setting;}
    namespace Exception  {Setting setting;}
    namespace Example    {Setting setting;}
}

Settings::Debug::Setting::Setting() :
        showDebugMessages(true),
        debugModes(DebugMode::inConsole | DebugMode::inFile)
{}

Settings::Exception::Setting::Setting() :
        showExceptionMessage(true)
{}

Settings::Example::Setting::Setting() :
        showTask1DebugMessage(true),
        showTask2DebugMessage(true),
        showTask3DebugMessage(true),
        showMainDebugMessage(true)
{}