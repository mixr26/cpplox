#ifndef __RETURN_H
#define __RETURN_H

#include <exception>

#include "literal.h"

class Return : public std::exception {
    Literal value;
public:
    Return(Literal value) : std::exception(), value(value)  {}
    Return(const Return&) = default;
    Return(Return&&) = default;
    ~Return() = default;
    Return& operator=(Return&) = default;
    Return& operator=(Return&&) = default;

    Literal get_value() { return value; }
};

#endif // __RETURN_H
