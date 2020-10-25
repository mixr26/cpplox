#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <memory>

#include "token.h"
#include "runtime_error.h"
#include "literal.h"

// Class describing a runtime environment.
class Environment {
    // Enclosing (parent) environment.
    std::shared_ptr<Environment> enclosing;
    // Map of defined values.
    std::unordered_map<std::string, Literal> values;
public:
    Environment() : enclosing(nullptr), values() {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}
    Environment(const Environment&) = delete;
    Environment(Environment&&) = delete;
    ~Environment() = default;
    Environment& operator=(Environment&) = delete;
    Environment& operator=(Environment&&) = delete;

    // Define a new variable.
    void define(std::string name, Literal value) { values[name] = value; }
    // Assign to an existing variable.
    void assign(std::shared_ptr<Token> name, Literal value);
    // Get the value of an existing variable.
    Literal get(std::shared_ptr<Token> name);
};

#endif // __ENVIRONMENT_H
