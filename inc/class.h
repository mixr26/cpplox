#ifndef __CLASS_H
#define __CLASS_H

#include <string>
#include <unordered_map>

#include "function.h"

// Represents a Lox class.
class Class : public Callable,
              public std::enable_shared_from_this<Class>{
public:
    using method_map = std::unordered_map<std::string, std::shared_ptr<Function>>;
private:
    std::string name;
    std::shared_ptr<Class> superclass;
    method_map methods;
public:
    Class(std::string name, std::shared_ptr<Class> superclass,
          method_map methods)
        : name(name), superclass(superclass), methods(methods) {}
    Class() = default;
    Class(const Class&) = delete;
    Class(Class&&) = delete;
    ~Class() = default;
    Class& operator=(Class&) = delete;
    Class& operator=(Class&&) = delete;

    // Invoke a call operator on the Callable instance (class or function).
    Literal call(std::shared_ptr<Interpreter> interpreter,
                 std::vector<Literal>& arguments) override;
    // Check the arity of the function.
    uint32_t arity() override;

    std::string get_name() { return name; }
    std::shared_ptr<Function> find_method(std::string name);
};

#endif // __CLASS_H
