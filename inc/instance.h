#ifndef __INSTANCE_H
#define __INSTANCE_H

#include <memory>
#include <unordered_map>

#include "literal.h"

class Class;
class Token;

// Describes a class instance.
class Instance : public std::enable_shared_from_this<Instance> {
    using fields_map = std::unordered_map<std::string, Literal>;

    std::shared_ptr<Class> klass;
    fields_map fields;
public:
    Instance(std::shared_ptr<Class> klass) : klass(klass) {}
    Instance();
    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;
    ~Instance() = default;
    Instance& operator=(Instance&) = delete;
    Instance& operator=(Instance&&) = delete;

    std::shared_ptr<Class> get_klass() { return klass; }
    Literal get(std::shared_ptr<Token> name);
    void set(std::shared_ptr<Token> name, Literal value);
};

#endif // __INSTANCE_H
