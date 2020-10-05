#include <iostream>

#include "error_handling.h"

namespace error_handling {

bool had_error = false;

void error(uint32_t line, std::string msg) {
    report(line, "", msg);
}

void error(std::shared_ptr<Token> tok, std::string msg) {
    if (tok->get_type() == TokenType::END)
        report(tok->get_line(), "at end", msg);
    else
        report(tok->get_line(), " at '" + tok->get_lexeme() + "'", msg);
}

void report(uint32_t line, std::string where, std::string msg) {
    std::cout << "[line " << line << "] Error" << where << ": "
              << msg << std::endl;
    had_error = true;
}

}
