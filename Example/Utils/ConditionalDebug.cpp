#include "ConditionalDebug.h"
#include "Settings.h"
#include "Exceptions.h"
#include <filesystem>
#include <memory>

std::mutex ConditionalDebug::mutex;
std::unique_ptr<FileHolder> ConditionalDebug::fileHolder;
using namespace Settings::Debug;

ConditionalDebug::ConditionalDebug(bool enable) :
        debugEnabled(enable && Settings::Debug::setting.showDebugMessages),
        debugInFile(debugEnabled && setting.debugModes.isFlagSet(Setting::DebugMode::inFile)),
        debugInConsole(debugEnabled && setting.debugModes.isFlagSet(Setting::DebugMode::inConsole))
{
    prepareDebugFile();
}

ConditionalDebug::ConditionalDebug(ConditionalDebug&& other) noexcept {
    debugEnabled = other.debugEnabled;
    debugInFile = other.debugInFile;
    debugInConsole = other.debugInConsole;
    space = other.space;
}

ConditionalDebug::~ConditionalDebug() {
    if (fileHolder) {
        std::lock_guard<std::mutex> guard(mutex);
        fileHolder->file << std::endl;
    }
}

ConditionalDebug& ConditionalDebug::noSpace() {
    space = false;
    return *this;
}

ConditionalDebug& ConditionalDebug::withSpace() {
    space = true;
    return *this;
}

void ConditionalDebug::prepareDebugFile() const {
    std::lock_guard<std::mutex> guard(mutex);
    if(debugInFile) {
        if (!fileHolder || (fileHolder && fileHolder->file.tellp() > maxFileSize)) {
            bool directorExists = std::filesystem::is_directory("DebugOutput");
            if (!directorExists) {
                bool directorCreated = std::filesystem::create_directory("DebugOutput");
                if(!directorCreated) {
                    throw FileIOException();
                }
            }
            fileHolder = std::make_unique<FileHolder>("DebugOutput/debug", "txt");
        }
    }
}
