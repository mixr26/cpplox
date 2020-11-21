#ifndef __LITERAL_H
#define __LITERAL_H

#include <variant>
#include <cstddef>
#include <string>
#include <ostream>
#include <memory>

class Callable;
class Function;
class Lambda;
class Class;
class Instance;

// POD class which represents a literal.
// Consists of a single variant of possible C++ types of the Lox literals.
struct Literal {
    std::variant<std::nullptr_t,
                 std::string,
                 double,
                 bool,
                 std::shared_ptr<Callable>,
                 std::shared_ptr<Function>,
                 std::shared_ptr<Lambda>,
                 std::shared_ptr<Class>,
                 std::shared_ptr<Instance>> value;

    friend std::ostream& operator<<(std::ostream& os, const Literal& lit);
};

#endif // __LITERAL_H
