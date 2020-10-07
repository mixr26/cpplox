#ifndef __TOKEN_H
#define __TOKEN_H

#include <cstdint>
#include <string>
#include <ostream>

// Enum class representing all token types.
enum class Token_type : uint8_t {
    // Single character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    // Literals.
    IDENTIFIER, STRING, NUMBER,
    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END
};

class Token {
    Token_type type;
    std::string lexeme;
    // Line on which the token appears.
    uint32_t line;
    // For NUMBER tokens, contains the real number value.
    double value;
public:
    Token(Token_type type, std::string lexeme, uint32_t line, double value = 0.)
        : type(type), lexeme(lexeme), line(line), value(value) {}
    Token(const Token&) = default;
    Token(Token&&) = default;
    ~Token() = default;
    Token& operator=(Token&) = default;
    Token& operator=(Token&&) = default;

    Token_type get_type() { return type; }
    const std::string& get_lexeme() const { return lexeme; }
    uint32_t get_line() { return line; }
    double get_value() { return value; }

    // Overloaded operator for printing the token.
    friend std::ostream& operator<<(std::ostream& os, const Token& tok);
};

#endif //__TOKEN_H
