#ifndef __PARSER_H
#define __PARSER_H

#include <string>
#include <list>

#include "token.h"
#include "tree.h"

class Parser {
    using token_iterator = std::list<std::shared_ptr<Token>>::iterator;

    // Custom parser exception class.
    class Parse_error : public std::exception {};

    // Reference to the stream of tokens acquired from the scanner.
    std::list<std::shared_ptr<Token>>& tokens;
    // Currently processed token.
    token_iterator current;

    // List of program statements.
    std::list<std::shared_ptr<Stmt>> statements;

    // If the next token matches the expected, advance the token stream.
    bool match(Token_type type);
    // Whether the next token matches the expected.
    bool check(Token_type type);
    // Advance the token stream.
    std::shared_ptr<Token> advance();
    // Whether the current token signalizes the end of the token stream.
    bool is_at_end();
    // Return the next token, but don't advance the stream.
    std::shared_ptr<Token> peek();
    // Return the previous token.
    std::shared_ptr<Token> previous();
    // Check whether the next token matches the expected and advance the stream
    // if it does. Conversely, throw an error.
    std::shared_ptr<Token> consume(Token_type type, std::string msg);

    // Parse the function call expression argument list.
    std::shared_ptr<Expr> finish_call(std::shared_ptr<Expr> callee);

    std::shared_ptr<Function_stmt> function(std::string kind);
    std::shared_ptr<Lambda_expr> lambda();

    // Discard the (possibly) erroneous tokens until we see one of the
    // synchronization tokens. Called after the parser reports an error.
    void synchronize();

    // Report an error and throw an exception.
    Parse_error error(std::shared_ptr<Token> tok, std::string msg);

    // Methods which represent the grammar nonterminals.
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> var_declaration();
    std::shared_ptr<Stmt> statement();
    std::list<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Stmt> if_statement();
    std::shared_ptr<Stmt> while_statement();
    std::shared_ptr<Stmt> for_statement();
    std::shared_ptr<Stmt> print_statement();
    std::shared_ptr<Stmt> return_statement();
    std::shared_ptr<Stmt> expression_statement();
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> logical_or();
    std::shared_ptr<Expr> logical_and();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> addition();
    std::shared_ptr<Expr> multiplication();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();
public:
    Parser(std::list<std::shared_ptr<Token>>&& tokens)
        : tokens(tokens), current(tokens.begin()) {}
    Parser(const Parser&) = delete;
    Parser(Parser&&) = delete;
    ~Parser() = default;
    Parser& operator=(Parser&) = delete;
    Parser& operator=(Parser&&) = delete;

    // Parse the token stream and return the root of the AST.
    std::list<std::shared_ptr<Stmt>>&& parse();
};

#endif // __PARSER_H
