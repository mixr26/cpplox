#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include <list>

#include "tree.h"
#include "literal.h"
#include "environment.h"

// Interpreter visitor class.
class Interpreter : public Expr_visitor,
                    public Stmt_visitor,
                    public std::enable_shared_from_this<Interpreter> {
    // Result of the interpreter run. Also holds the intermediate results
    // during the interpreter run.
    Literal result;
    std::shared_ptr<Environment> environment;

    // Evaluate an expression. Just a wrapper around the call to accept method.
    void evaluate(std::shared_ptr<Expr> expr);
    // Execute a statement. Just a wrapper around the call to accept method.
    void execute(std::shared_ptr<Stmt> stmt);
    void execute_block(std::list<std::shared_ptr<Stmt>>& statements,
                       std::shared_ptr<Environment> environment);
    // Is the literal considered to be TRUE.
    bool is_truthy();
    // Add two literals.
    void add(auto& left);
    // Are two literals equal.
    bool is_equal(Literal& left);
public:
    Interpreter() : result(), environment(std::make_shared<Environment>()) {}
    Interpreter(const Interpreter&) = delete;
    Interpreter(Interpreter&&) = delete;
    ~Interpreter() = default;
    Interpreter& operator=(Interpreter&) = delete;
    Interpreter& operator=(Interpreter&&) = delete;

    // Implementation of expression visitor interface.
    void visit_literal_expr(const std::shared_ptr<Literal_expr> expr) override;
    void visit_grouping_expr(const std::shared_ptr<Grouping_expr> expr) override;
    void visit_unary_expr(const std::shared_ptr<Unary_expr> expr) override;
    void visit_binary_expr(const std::shared_ptr<Binary_expr> expr) override;
    void visit_variable_expr(const std::shared_ptr<Variable_expr> expr) override;
    void visit_assign_expr(const std::shared_ptr<Assign_expr> expr) override;

    // Implementation of statement visitor interface.
    void visit_expression_stmt(const std::shared_ptr<Expression_stmt> stmt) override;
    void visit_print_stmt(const std::shared_ptr<Print_stmt> stmt) override;
    void visit_var_stmt(const std::shared_ptr<Var_stmt> stmt) override;
    void visit_block_stmt(const std::shared_ptr<Block_stmt> stmt) override;

    // Start the interpreter run.
    void interpret(std::list<std::shared_ptr<Stmt>>&& statements);
    // Get the result of the interpreter run.
    Literal get_result() { return result; }
};

#endif // __INTERPRETER_H
