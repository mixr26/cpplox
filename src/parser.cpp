#include "parser.h"
#include "error_handling.h"

// If the next token matches the expected, advance the token stream.
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }

    return false;
}

// Whether the next token matches the expected.
bool Parser::check(TokenType type) {
    if (is_at_end())
        return false;
    return peek()->get_type() == type;
}

// Advance the token stream.
std::shared_ptr<Token> Parser::advance() {
    if (!is_at_end())
        current++;
    return previous();
}

// Whether the current token signalizes the end of the token stream.
bool Parser::is_at_end() {
    return peek()->get_type() == TokenType::END;
}

// Return the next token, but don't advance the stream.
std::shared_ptr<Token> Parser::peek() {
    return *current;
}

// Return the previous token.
std::shared_ptr<Token> Parser::previous() {
    token_iterator help = --current;
    current++;

    return *help;
}

// Check whether the next token matches the expected and advance the stream
// if it does. Conversely, throw an error.
std::shared_ptr<Token> Parser::consume(TokenType type, std::string msg) {
    if (check(type))
        return advance();

    throw error(peek(), msg);
}

// Discard the (possibly) erroneous tokens until we see one of the
// synchronization tokens. Called after the parser reports an error.
void Parser::synchronize() {
    advance();

    while (!is_at_end()) {
        if (previous()->get_type() == TokenType::SEMICOLON)
            return;

        switch (peek()->get_type()) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
        default:
            return;
        }
    }
}

// Report an error and throw an exception.
Parser::Parse_error Parser::error(std::shared_ptr<Token> tok, std::string msg) {
    error_handling::error(tok, msg);
    return Parse_error();
}

// Methods which represent the grammar nonterminals.

std::shared_ptr<Expr> Parser::expression() {
    return equality();
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();

    while (match(TokenType::BANG_EQUAL)
           || match(TokenType::EQUAL_EQUAL)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = addition();

    while (match(TokenType::GREATER)
           || match(TokenType::GREATER_EQUAL)
           || match(TokenType::LESS)
           || match(TokenType::LESS_EQUAL)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = addition();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::addition() {
    std::shared_ptr<Expr> expr = multiplication();

    while (match(TokenType::MINUS)
           || match(TokenType::PLUS)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = multiplication();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::multiplication() {
    std::shared_ptr<Expr> expr = unary();

    while (match(TokenType::SLASH)
           || match(TokenType::STAR)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match(TokenType::BANG)
        || match(TokenType::MINUS)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary_expr>(right, op);
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(TokenType::FALSE)
        || match(TokenType::TRUE)
        || match(TokenType::NIL)
        || match(TokenType::STRING)
        || match(TokenType::NUMBER))
        return std::make_shared<Literal_expr>(previous());
    else if (match(TokenType::LEFT_PAREN)) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression!");
        return std::make_shared<Grouping_expr>(expr);
    } else
        throw error(peek(), "Expect expression!");
}

// Parse the token stream and return the root of the AST.
std::shared_ptr<Expr> Parser::parse() {
    try {
        return expression();
    } catch (Parse_error err) {
        return nullptr;
    }
}
