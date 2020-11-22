#include "class.h"
#include "instance.h"

Literal Class::call(std::shared_ptr<Interpreter> interpreter,
                    std::vector<Literal> &arguments) {
    Literal ret;
    auto instance = std::make_shared<Instance>(shared_from_this());

    std::shared_ptr<Function> initializer = find_method("init");
    if (initializer != nullptr)
        initializer->bind(instance)->call(interpreter, arguments);

    ret.value = instance;
    return ret;
}

uint32_t Class::arity() {
    std::shared_ptr<Function> initializer = find_method("init");
    if (initializer == nullptr)
        return 0;

    return initializer->arity();
}

std::shared_ptr<Function> Class::find_method(std::string name) {
    if (methods.find(name) != methods.end())
        return methods[name];

    if (superclass != nullptr)
        return superclass->find_method(name);

    return nullptr;
}
