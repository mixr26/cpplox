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
