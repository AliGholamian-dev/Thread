#include "FileHolder.h"
#include "Exceptions.h"
#include <filesystem>

using namespace std;

FileHolder::FileHolder(const std::string& filenameNoSuffix, const std::string& suffix) :
        filenameNoSuffix(filenameNoSuffix),
        suffixWithDot(suffix)
{
    suffixWithDot = "." + suffixWithDot;
    findNonExistingFileName();
    openFile();
}

FileHolder::~FileHolder() {
    closeFile();
}

void FileHolder::findNonExistingFileName() {
    filename = filenameNoSuffix + suffixWithDot;
    if (std::filesystem::exists(filename)) {
        int fileNumber = 0;
        filenameNoSuffix += "_";
        while (std::filesystem::exists(filenameNoSuffix + std::to_string(fileNumber) + suffixWithDot)) {
            fileNumber++;
        }
        filename = filenameNoSuffix + std::to_string(fileNumber) + suffixWithDot;
    }
}

void FileHolder::openFile() {
    file.open(filename);
    if (!file)
        throw FileIOException();
}

void FileHolder::closeFile() {
    if(file.is_open()) {
        file.close();
    }
}