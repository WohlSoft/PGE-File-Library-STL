#pragma once

#include <string>

class PGEStringUtils
{
public:
    PGEStringUtils() = delete;
    static inline bool strEndsWith(std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else {
            return false;
        }
    }
};

