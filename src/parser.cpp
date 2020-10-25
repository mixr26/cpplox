#include <typeinfo>

#include "parser.h"
#include "error_handling.h"

// If the next token matches the expected, advance the token stream.
bool Parser::match(Token_type type) {
    if (check(type)) {
        advance();
        return true;
    }

    return false;
}

// Whether the next token matches the expected.
bool Parser::check(Token_type type) {
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
    return peek()->get_type() == Token_type::END;
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
std::shared_ptr<Token> Parser::consume(Token_type type, std::string msg) {
    if (check(type))
        return advance();

    throw error(peek(), msg);
}

// Discard the (possibly) erroneous tokens until we see one of the
// synchronization tokens. Called after the parser reports an error.
void Parser::synchronize() {
    advance();

    while (!is_at_end()) {
        if (previous()->get_type() == Token_type::SEMICOLON)
            return;

        switch (peek()->get_type()) {
        case Token_type::CLASS:
        case Token_type::FUN:
        case Token_type::VAR:
        case Token_type::FOR:
        case Token_type::IF:
        case Token_type::WHILE:
        case Token_type::PRINT:
        case Token_type::RETURN:
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

std::shared_ptr<Stmt> Parser::declaration() {
    try {
        if (match(Token_type::VAR))
            return var_declaration();
        return statement();
    } catch (Parse_error& e) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::var_declaration() {
    std::shared_ptr<Token> name = consume(Token_type::IDENTIFIER,
                                          "Expect variable name!");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match(Token_type::EQUAL))
        initializer = expression();

    consume(Token_type::SEMICOLON, "Expect ';' after variable declaration!");
    return std::make_shared<Var_stmt>(name, initializer);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match(Token_type::PRINT))
        return print_statement();
    if (match(Token_type::LEFT_BRACE))
        return std::make_shared<Block_stmt>(block());
    return expression_statement();
}

std::shared_ptr<Stmt> Parser::print_statement() {
    std::shared_ptr<Expr> expr = expression();
    consume(Token_type::SEMICOLON, "Expect ';' after value!");
    return std::make_shared<Print_stmt>(expr);
}

std::shared_ptr<Stmt> Parser::expression_statement() {
    std::shared_ptr<Expr> expr = expression();
    consume(Token_type::SEMICOLON, "Expect ';' after value!");
    return std::make_shared<Expression_stmt>(expr);
}

std::list<std::shared_ptr<Stmt>> Parser::block() {
    std::list<std::shared_ptr<Stmt>> statements;

    while (!check(Token_type::RIGHT_BRACE) && !is_at_end())
        statements.emplace_back(declaration());

    consume(Token_type::RIGHT_BRACE, "Expect '}' after block!");
    return statements;
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = equality();

    if (match(Token_type::EQUAL)) {
        std::shared_ptr<Token> equals = previous();
        std::shared_ptr<Expr> value = assignment();

        std::shared_ptr<Expr> assign = expr->make_assignment_expr(expr, value);
        if (assign != nullptr)
            return assign;
        error(equals, "Invalid assignment target!");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();

    while (match(Token_type::BANG_EQUAL)
           || match(Token_type::EQUAL_EQUAL)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = addition();

    while (match(Token_type::GREATER)
           || match(Token_type::GREATER_EQUAL)
           || match(Token_type::LESS)
           || match(Token_type::LESS_EQUAL)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = addition();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::addition() {
    std::shared_ptr<Expr> expr = multiplication();

    while (match(Token_type::MINUS)
           || match(Token_type::PLUS)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = multiplication();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::multiplication() {
    std::shared_ptr<Expr> expr = unary();

    while (match(Token_type::SLASH)
           || match(Token_type::STAR)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary_expr>(expr, right, op);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match(Token_type::BANG)
        || match(Token_type::MINUS)) {
        std::shared_ptr<Token> op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary_expr>(right, op);
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(Token_type::FALSE)
        || match(Token_type::TRUE)
        || match(Token_type::NIL)
        || match(Token_type::STRING)
        || match(Token_type::NUMBER))
        return std::make_shared<Literal_expr>(previous());
    else if (match(Token_type::IDENTIFIER))
        return std::make_shared<Variable_expr>(previous());
    else if (match(Token_type::LEFT_PAREN)) {
        std::shared_ptr<Expr> expr = expression();
        consume(Token_type::RIGHT_PAREN, "Expect ')' after expression!");
        return std::make_shared<Grouping_expr>(expr);
    } else
        throw error(peek(), "Expect expression!");
}

// Parse the token stream and return the root of the AST.
std::list<std::shared_ptr<Stmt>>&& Parser::parse() {
    while (!is_at_end())
        statements.emplace_back(declaration());
    return std::move(statements);
}
