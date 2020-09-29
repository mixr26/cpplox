#include <iostream>

#include "error_handling.h"

bool had_error = false;

void error(uint32_t line, std::string msg) {
    report(line, "", msg);
}

void report(uint32_t line, std::string where, std::string msg) {
    std::cout << "[line " << line << "] Error" << where << ": "
              << msg << std::endl;
    had_error = true;
}
