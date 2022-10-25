#pragma once

#include "Settings.h"
#include "ConditionalDebug.h"
#include <exception>
#include <utility>

class Exception : public std::exception {
    public:
        explicit Exception(const std::string& message) : std::exception(message.c_str()) {
            printExceptionMessage(message);
        }

    private:
        static void printExceptionMessage(const std::string& message) {
            ConditionalDebug debug(Settings::Exception::setting.showExceptionMessage);
            debug << std::endl << "-----------Exception Occurred-----------" << std::endl;
            debug << message;
            debug << std::endl << "----------------------------------------" << std::endl;
        }
};

class NullptrException final : public Exception {
    public:
        NullptrException() : Exception("Null Pointer Exception") {}
};


class UnexpectedException final : public Exception {
    public:
        UnexpectedException() : Exception("Unexpected Exception") {}
};

class  WrongParametersException final : public Exception {
    public:
        WrongParametersException() : Exception("Wrong Parameters Exception") {}
};

class FileIOException final : public Exception {
    public:
        FileIOException() : Exception("File IO Exception") {}
};

class NotImplementedException final : public Exception {
    public:
        NotImplementedException() : Exception("Not Implemented Exception") {}
};

class ItemIsUsedBeforeException final : public Exception {
    public:
        ItemIsUsedBeforeException() : Exception("Item Is Used Before Exception") {}
};