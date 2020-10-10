#ifndef __LITERAL_H
#define __LITERAL_H

#include <variant>
#include <cstddef>
#include <string>
#include <ostream>

// POD class which represents a literal.
// Consists of a single variant of possible C++ types of the Lox literals.
struct Literal {
    std::variant<std::nullptr_t, std::string, double, bool> value;

    friend std::ostream& operator<<(std::ostream& os, const Literal& lit);
};

#endif // __LITERAL_H
