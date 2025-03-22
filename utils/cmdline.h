#pragma once

#include <string>

namespace dmr { 
    namespace util {

    char *getCmdOption(char ** begin, char ** end, const std::string& option);
    bool cmdOptionExists(char** begin, char** end, const std::string& option);
    }
}
