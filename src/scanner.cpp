#include "scanner.h"
#include "error_handling.h"

// Hash map of the language's keywords.
const Scanner::keywords_map Scanner::keywords = {
    {"and" , Token_type::AND},
    {"class", Token_type::CLASS},
    {"else", Token_type::ELSE},
    {"false", Token_type::FALSE},
    {"for", Token_type::FOR},
    {"fun", Token_type::FUN},
    {"if", Token_type::IF},
    {"nil", Token_type::NIL},
    {"or", Token_type::OR},
    {"print", Token_type::PRINT},
    {"return", Token_type::RETURN},
    {"super", Token_type::SUPER},
    {"this", Token_type::THIS},
    {"true", Token_type::TRUE},
    {"var", Token_type::VAR},
    {"while", Token_type::WHILE}
};

// Scanner constructor. Opens the input stream and checks the size of the
// source file.
Scanner::Scanner(std::string source)
    : source(source), current(0U), line(1U) {
    filestream.open(source);
    if (!filestream.is_open()) {
        error_handling::had_error = true;
        error_handling::error(line, "Input file not opened correctly!");
        exit(1);
    }

    // Check the source file size.
    struct stat stat_buf;
    int rc = stat(source.c_str(), &stat_buf);
    source_size = rc == 0 ? stat_buf.st_size - 1 : 0U;
}

// Whether the next character in the input stream matches the expected.
bool Scanner::match(char expected) {
    if (is_at_end())
        return false;
    if (static_cast<char>(filestream.peek()) != expected)
        return false;

    return true;
}

// Helper method which recognizes string literals.
void Scanner::string_lit() {
    std::string lexeme = "";

    // Munch the characters until the end of the current line.
    while (!match('"') && !is_at_end()) {
        if (match('\n'))
            line++;
        lexeme.push_back(advance());
    }

    // Unterminated string.
    if (is_at_end()) {
        error_handling::error(line, "Unterminated string!");
        return;
    }

    // The closing ".
    advance();

    add_token(Token_type::STRING, lexeme);
}

// Helper method which recognizes number literals.
void Scanner::num_lit() {
    std::string lexeme = "";

    // Munch the digits.
    while (is_digit(static_cast<char>(filestream.peek())))
        lexeme.push_back(advance());

    // Look for a fractional part.
    if (match('.')) {
        // Consume the "."
        lexeme.push_back(advance());

        // Munch the fractional part.
        while (is_digit(static_cast<char>(filestream.peek())))
            lexeme.push_back(advance());
    }

    add_token(Token_type::NUMBER, lexeme, atof(lexeme.c_str()));
}

// Helper method which recognizes keywords and identifiers.
void Scanner::identifier() {
    std::string lexeme = "";
    // Munch the first character (which mustn't be a digit).
    lexeme.push_back(advance());

    // Munch the characters.
    while (is_alphanum(static_cast<char>(filestream.peek())))
        lexeme.push_back(advance());

    // If this lexeme matches a keyword, set the matching token type.
    auto type = keywords.find(lexeme);
    if (type == keywords.end())
        add_token(Token_type::IDENTIFIER, lexeme);
    else
        add_token(type->second, lexeme);
}

// Helper method which extracts one token from the source file.
void Scanner::scan_token() {
    // Munch the first character.
    char c = advance();

    switch (c) {
    case '(': add_token(Token_type::LEFT_PAREN, "("); break;
    case ')': add_token(Token_type::RIGHT_PAREN, ")"); break;
    case '{': add_token(Token_type::LEFT_BRACE, "{"); break;
    case '}': add_token(Token_type::RIGHT_BRACE, "}"); break;
    case ',': add_token(Token_type::COMMA, ","); break;
    case '.': add_token(Token_type::DOT, "."); break;
    case '-': add_token(Token_type::MINUS, "-"); break;
    case '+': add_token(Token_type::PLUS, "+"); break;
    case ';': add_token(Token_type::SEMICOLON, ";"); break;
    case '*': add_token(Token_type::STAR, "*"); break;
    case '!':
        if (match('=')) {
            advance();
            add_token(Token_type::BANG_EQUAL, "!=");
        } else
            add_token(Token_type::BANG, "!");
        break;
    case '=':
        if (match('=')) {
            advance();
            add_token(Token_type::EQUAL_EQUAL, "==");
        } else
            add_token(Token_type::EQUAL, "=");
        break;
    case '<':
        if (match('=')) {
            advance();
            add_token(Token_type::LESS_EQUAL, "<=");
        } else
            add_token(Token_type::LESS, "<");
        break;
    case '>':
        if (match('=')) {
            advance();
            add_token(Token_type::GREATER_EQUAL, ">=");
        } else
            add_token(Token_type::GREATER, ">");
        break;
    case '/':
        if (match('/')) {
            while(!match('\n') && !is_at_end())
                advance();
        } else
            add_token(Token_type::SLASH, "/");
        break;
    case '\n': line++; break;
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;
    case '"': string_lit(); break;
    case '0' ... '9': rollback(); num_lit(); break;
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_':
        rollback();
        identifier();
        break;
    default: error_handling::error(line, "Unexpected character!"); break;
    }
}

// Begin the scanning process.
std::list<std::shared_ptr<Token>>&& Scanner::scan_tokens() {
    while (!is_at_end()) {
        // We are at the beginning of the next lexeme.
        scan_token();
    }

    // END token signalizes the end of the token stream.
    add_token(Token_type::END, "", 0);
    return std::move(tokens);
}
