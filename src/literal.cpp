#include "literal.h"

// Overloaded ostream operator for printing out the literal.
std::ostream& operator<<(std::ostream& os, const Literal& lit) {
    std::visit([&os = os](auto& arg){
        using T = std::decay_t<decltype(arg)>;
        if constexpr(std::is_same_v<T, std::nullptr_t>)
            os << "nil";
        else if constexpr(std::is_same_v<T, bool>)
            if (arg)
                os << "true";
            else
                os << "false";
        else
            os << arg;
    }, lit.value);

    return os;
}
