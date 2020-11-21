#ifndef __LAMBDA_H
#define __LAMBDA_H

#include <vector>

#include "callable.h"

class Lambda_expr;
class Environment;

// Represents an anonymous function.
class Lambda : public Callable {
    std::shared_ptr<Lambda_expr> declaration = nullptr;
    std::shared_ptr<Environment> closure;
public:
    // Invoke a call operator on the Callable instance.
    Literal call(std::shared_ptr<Interpreter> interpreter,
                 std::vector<Literal>& arguments) override;
    // Check the arity of the function.
    uint32_t arity() override;

    Lambda(std::shared_ptr<Lambda_expr> declaration,
           std::shared_ptr<Environment> closure)
        : declaration(declaration), closure(closure) {}
    Lambda() = default;
    Lambda(const Lambda&) = delete;
    Lambda(Lambda&&) = delete;
    ~Lambda() = default;
    Lambda& operator=(Lambda&) = delete;
    Lambda& operator=(Lambda&&) = delete;
};

#endif // __LAMBDA_H
