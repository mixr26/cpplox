#include "token.h"

// Overloaded operator for printing the token.
std::ostream& operator<<(std::ostream& os, const Token& tok) {
    os << static_cast<uint32_t>(tok.type) << " " << tok.lexeme << std::endl;
    return os;
}
