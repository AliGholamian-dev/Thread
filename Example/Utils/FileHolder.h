#pragma once

#include <fstream>
#include <string>


class FileHolder final {
    public:
        FileHolder(const std::string& filenameNoSuffix, const std::string& suffix);
        ~FileHolder();
        void closeFile();
        std::ofstream file;

    private:
        void findNonExistingFileName();
        void openFile();

        std::string filenameNoSuffix;
        std::string suffixWithDot;
        std::string filename;
};



