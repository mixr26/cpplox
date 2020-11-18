#include "environment.h"

// Get the value of an existing variable.
Literal Environment::get(std::shared_ptr<Token> name) {
    if (values.count(name->get_lexeme()) > 0)
        return values.at(name->get_lexeme());

    if (enclosing != nullptr)
        return enclosing->get(name);

    throw Runtime_error("Undefined variable " + name->get_lexeme() + "!",
                        name);
}

// Assign to an existing variable.
void Environment::assign(std::shared_ptr<Token> name, Literal value) {
    if (values.count(name->get_lexeme()) > 0) {
        values[name->get_lexeme()] = value;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw Runtime_error("Undefined variable " + name->get_lexeme() + "!",
                        name);
}

// Get the value of an existing variable, at the desired depth in the
// environment stack.
Literal Environment::get_at(int distance, std::string name) {
    return ancestor(distance)->values[name];
}

// Get the environment at the desired depth.
std::shared_ptr<Environment> Environment::ancestor(int distance) {
    std::shared_ptr<Environment> environment = shared_from_this();
    for (int i = 0; i < distance; i++) {
        environment = environment->enclosing;
    }

    return environment;
}

// Set the value of an existing variable, at the desired depth in the
// environment stack.
void Environment::assign_at(int distance, std::shared_ptr<Token> name,
                            Literal value) {
    ancestor(distance)->values[name->get_lexeme()] = value;
}
