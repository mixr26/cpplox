#include "function.h"
#include "environment.h"
#include "interpreter.h"
#include "return.h"

// Invoke a call operator on the function
Literal Function::call(std::shared_ptr<Interpreter> interpreter,
                       std::vector<Literal>& arguments) {
    std::shared_ptr<Environment> environment
            = std::make_shared<Environment>(interpreter->globals);
    for (unsigned int i = 0; i < declaration->get_params().size(); i++) {
        environment->define(((declaration->get_params()).at(i)->get_lexeme()),
                            arguments.at(i));
    }

    try {
        interpreter->execute_block(declaration->get_body(), environment);
    } catch (Return return_value) {
        return return_value.get_value();
    }

    Literal ret;
    ret.value = nullptr;
    return ret;
}

// Check the arity of the function.
uint32_t Function::arity() { return (declaration->get_params()).size(); }
