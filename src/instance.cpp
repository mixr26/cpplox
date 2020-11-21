#include "instance.h"
#include "token.h"
#include "class.h"
#include "runtime_error.h"

Literal Instance::get(std::shared_ptr<Token> name) {
    if (fields.find(name->get_lexeme()) != fields.end())
        return fields[name->get_lexeme()];

    std::shared_ptr<Function> method = klass->find_method(name->get_lexeme());
    if (method != nullptr) {
        Literal ret;
        ret.value = method->bind(shared_from_this());
        return ret;
    }

    throw Runtime_error("Undefined property '" + name->get_lexeme() + "'.",
                        name);
}

void Instance::set(std::shared_ptr<Token> name, Literal value) {
    fields[name->get_lexeme()] = value;
}
