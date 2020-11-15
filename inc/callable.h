#ifndef __CALLABLE_H
#define __CALLABLE_H

#include <memory>
#include <vector>

#include "literal.h"

class Interpreter;

class Callable {
public:
    // Invoke a call operator on the Callable instance (class or function).
    virtual Literal call(std::shared_ptr<Interpreter> interpreter,
                         std::vector<Literal>& arguments) = 0;
    // Check the arity of the function.
    virtual uint32_t arity() = 0;

    Callable() = default;
    Callable(const Callable&) = delete;
    Callable(Callable&&) = delete;
    virtual ~Callable() = default;
    Callable& operator=(Callable&) = delete;
    Callable& operator=(Callable&&) = delete;
};

#endif // __CALLABLE_H
