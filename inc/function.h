#ifndef __LOX_FUNCTION_H
#define __LOX_FUNCTION_H

#include <vector>

#include "callable.h"

class Function_stmt;
class Environment;

class Function : public Callable {
    std::shared_ptr<Function_stmt> declaration = nullptr;
    std::shared_ptr<Environment> closure;
public:
    // Invoke a call operator on the Callable instance (class or function).
    Literal call(std::shared_ptr<Interpreter> interpreter,
                 std::vector<Literal>& arguments) override;
    // Check the arity of the function.
    uint32_t arity() override;

    Function(std::shared_ptr<Function_stmt> declaration,
             std::shared_ptr<Environment> closure)
        : declaration(declaration), closure(closure) {}
    Function() = default;
    Function(const Function&) = delete;
    Function(Function&&) = delete;
    ~Function() = default;
    Function& operator=(Function&) = delete;
    Function& operator=(Function&&) = delete;
};

#endif // __LOX_FUNCTION_H
