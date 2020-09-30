#ifndef __SCANNER_H
#define __SCANNER_H

#include <list>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unordered_map>
#include <memory>

#include "error_handling.h"

// Enum class representing all token types.
enum class TokenType : uint8_t {
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
    TokenType type;
    std::string lexeme;
    // Line on which the token appears.
    uint32_t line;
    // For NUMBER tokens, contains the real number value.
    double value;
public:
    Token(TokenType type, std::string lexeme, uint32_t line, double value = 0.)
        : type(type), lexeme(lexeme), line(line), value(value) {}
    Token(const Token&) = default;
    Token(Token&&) = default;
    ~Token() = default;
    Token& operator=(Token&) = default;
    Token& operator=(Token&&) = default;

    // Overloaded operator for printing the token.
    friend std::ostream& operator<<(std::ostream& os, const Token& tok);
};

class Scanner {
    using keywords_map = std::unordered_map<std::string, TokenType>;

    // Name of the source file.
    std::string source;
    // Stream of valid tokens.
    std::list<std::shared_ptr<Token>> tokens;

    // Current character number.
    uint32_t current;
    // Current source file line being processed.
    uint32_t line;

    // Source file stream.
    std::ifstream filestream;
    // Size of the source file (excluding EOF character).
    uint64_t source_size;

    // Hash map of the language's keywords.
    static const keywords_map keywords;

    // Helper method which extracts one token from the source file.
    void scan_token();

    // Whether the scanner has reached the end of the source file.
    constexpr bool is_at_end() { return current >= source_size; }
    // Advance the input stream.
    char advance() { current++; return static_cast<char>(filestream.get()); }
    // Roll one character back in the input stream.
    void rollback() { filestream.unget(); current--; }
    // Whether the next character in the input stream matches the expected.
    bool match(char expected);

    // Helper method which recognizes string literals.
    void string_lit();
    // Helper method which recognizes number literals.
    void num_lit();
    // Helper method which recognizes keywords and identifiers.
    void identifier();

    bool is_digit(char c) { return c >= '0' && c <= '9'; }
    bool is_alpha(char c) {
        return (c >= 'a' && c <= 'z')
               || (c >= 'A' && c <= 'Z')
               || c == '_';
    }
    bool is_alphanum(char c) { return is_digit(c) || is_alpha(c); }

    // Add new token to the token stream.
    void add_token(TokenType type, std::string lexeme) {
        tokens.emplace_back(std::make_shared<Token>(type, lexeme, line));
    }
    void add_token(TokenType type, std::string lexeme, double value) {
        tokens.emplace_back(std::make_shared<Token>(type, lexeme, line, value));
    }
public:
    Scanner(std::string source);
    Scanner(const Scanner&) = delete;
    Scanner(Scanner&&) = delete;
    ~Scanner() { filestream.close(); }
    Scanner& operator=(Scanner&) = delete;
    Scanner& operator=(Scanner&&) = delete;

    // Begin the scanning process.
    std::list<std::shared_ptr<Token>>& scan_tokens();
};

#endif // __SCANNER_H
