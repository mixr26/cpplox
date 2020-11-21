#ifndef __LOX_FUNCTION_H
#define __LOX_FUNCTION_H

#include <vector>

#include "callable.h"

class Function_stmt;
class Environment;

// Represents a Lox function.
class Function : public Callable {
    std::shared_ptr<Function_stmt> declaration = nullptr;
    std::shared_ptr<Environment> closure;
    bool is_initializer;
public:
    // Invoke a call operator on the Callable instance (class or function).
    Literal call(std::shared_ptr<Interpreter> interpreter,
                 std::vector<Literal>& arguments) override;
    // Check the arity of the function.
    uint32_t arity() override;
    // Bind a class instance to the class method invocation.
    std::shared_ptr<Function> bind(std::shared_ptr<Instance> instance);

    Function(std::shared_ptr<Function_stmt> declaration,
             std::shared_ptr<Environment> closure,
             bool is_initializer)
        : declaration(declaration), closure(closure),
          is_initializer(is_initializer) {}
    Function() = default;
    Function(const Function&) = delete;
    Function(Function&&) = delete;
    ~Function() = default;
    Function& operator=(Function&) = delete;
    Function& operator=(Function&&) = delete;
};

#endif // __LOX_FUNCTION_H
