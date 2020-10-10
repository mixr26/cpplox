#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include "tree.h"
#include "literal.h"

// Interpreter visitor class.
class Interpreter : public Expr_visitor,
                    public std::enable_shared_from_this<Interpreter> {
    // Result of the interpreter run. Also holds the intermediate results
    // during the interpreter run.
    Literal result;

    // Evaluate an expression. Just a wrapper around the call to accept method.
    void evaluate(std::shared_ptr<Expr> expr);
    // Is the literal considered to be TRUE.
    bool is_truthy();
    // Add two literals.
    void add(auto& left);
    // Are two literals equal.
    bool is_equal(Literal& left);
public:
    Interpreter() = default;
    Interpreter(const Interpreter&) = delete;
    Interpreter(Interpreter&&) = delete;
    ~Interpreter() = default;
    Interpreter& operator=(Interpreter&) = delete;
    Interpreter& operator=(Interpreter&&) = delete;

    // Implementation of visitor interface.
    void visit_literal_expr(const std::shared_ptr<Literal_expr> expr) override;
    void visit_grouping_expr(const std::shared_ptr<Grouping_expr> expr) override;
    void visit_unary_expr(const std::shared_ptr<Unary_expr> expr) override;
    void visit_binary_expr(const std::shared_ptr<Binary_expr> expr) override;

    // Start the interpreter run.
    void interpret(std::shared_ptr<Expr> expr);
    // Get the result of the interpreter run.
    Literal get_result() { return result; }
};

class Runtime_error : public std::runtime_error {
    std::shared_ptr<Token> token;
public:
    Runtime_error(std::string msg, std::shared_ptr<Token> token)
        : std::runtime_error(msg), token(token) {}
    Runtime_error(const Runtime_error&) = default;
    Runtime_error(Runtime_error&&) = default;
    ~Runtime_error() = default;
    Runtime_error& operator=(Runtime_error&) = default;
    Runtime_error& operator=(Runtime_error&&) = default;

    std::shared_ptr<Token> get_token() { return token; }
};

#endif // __INTERPRETER_H
