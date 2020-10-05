#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#include <string>
#include <memory>

#include "token.h"

// Whether we've seen an error.
extern bool had_error;

// Error reporting functions.
void error(uint32_t line, std::string msg);
void error(std::shared_ptr<Token> tok, std::string msg);
void report(uint32_t line, std::string where, std::string msg);

#endif // __ERROR_HANDLING_H
