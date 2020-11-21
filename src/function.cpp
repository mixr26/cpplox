#include "function.h"
#include "environment.h"
#include "interpreter.h"
#include "return.h"

// Invoke a call operator on the function
Literal Function::call(std::shared_ptr<Interpreter> interpreter,
                       std::vector<Literal>& arguments) {
    std::shared_ptr<Environment> environment
            = std::make_shared<Environment>(closure);
    for (unsigned int i = 0; i < declaration->get_params().size(); i++) {
        environment->define(((declaration->get_params()).at(i)->get_lexeme()),
                            arguments.at(i));
    }

    try {
        interpreter->execute_block(declaration->get_body(), environment);
    } catch (Return return_value) {
        if (is_initializer)
            return closure->get_at(0, "this");

        return return_value.get_value();
    }

    Literal ret;
    if (is_initializer)
        ret = closure->get_at(0, "this");
    else
        ret.value = nullptr;
    return ret;
}

// Check the arity of the function.
uint32_t Function::arity() { return (declaration->get_params()).size(); }

// Bind a class instance to the class method invocation.
std::shared_ptr<Function> Function::bind(std::shared_ptr<Instance> instance) {
    std::shared_ptr<Environment> environment
            = std::make_shared<Environment>(closure);

    Literal inst;
    inst.value = instance;
    environment->define("this", inst);

    return std::make_shared<Function>(declaration, environment, is_initializer);
}
