#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#include <string>

// Whether we've seen an error.
extern bool had_error;

// Error reporting functions.
void error(uint32_t line, std::string msg);
void report(uint32_t line, std::string where, std::string msg);

#endif // __ERROR_HANDLING_H
