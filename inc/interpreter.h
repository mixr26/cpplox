#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include <list>
#include <vector>
#include <unordered_map>

#include "tree.h"
#include "literal.h"
#include "environment.h"

// Interpreter visitor class.
class Interpreter : public Expr_visitor,
                    public Stmt_visitor,
                    public std::enable_shared_from_this<Interpreter> {
    using side_table = std::unordered_map<std::shared_ptr<Expr>, int>;

    friend class Function;
    friend class Lambda;

    // Result of the interpreter run. Also holds the intermediate results
    // during the interpreter run.
    Literal result;

    std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = globals;

    // Resolved scopes of expressions.
    side_table locals;

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
    // Get the Callable class (and its children) instance.
    std::shared_ptr<Callable> get_callable(Literal &callee, std::shared_ptr<Token> parent);
    // Get the Instance class instance.
    std::shared_ptr<Instance> get_instance(Literal& callee, std::shared_ptr<Token> parent);
    // Get the superclass of a class.
    std::shared_ptr<Class> get_superclass(Literal& callee, std::shared_ptr<Token> parent);
    // Look up a variable using the resolved depth.
    Literal look_up_variable(std::shared_ptr<Token> name, std::shared_ptr<Expr> expr);
public:
    Interpreter();
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
    void visit_logical_expr(const std::shared_ptr<Logical_expr> expr) override;
    void visit_call_expr(const std::shared_ptr<Call_expr> expr) override;
    void visit_lambda_expr(const std::shared_ptr<Lambda_expr> expr) override;
    void visit_get_expr(const std::shared_ptr<Get_expr> expr) override;
    void visit_set_expr(const std::shared_ptr<Set_expr> expr) override;
    void visit_this_expr(const std::shared_ptr<This_expr> expr) override;
    void visit_super_expr(const std::shared_ptr<Super_expr> expr) override;

    // Implementation of statement visitor interface.
    void visit_expression_stmt(const std::shared_ptr<Expression_stmt> stmt) override;
    void visit_print_stmt(const std::shared_ptr<Print_stmt> stmt) override;
    void visit_var_stmt(const std::shared_ptr<Var_stmt> stmt) override;
    void visit_block_stmt(const std::shared_ptr<Block_stmt> stmt) override;
    void visit_if_stmt(const std::shared_ptr<If_stmt> stmt) override;
    void visit_while_stmt(const std::shared_ptr<While_stmt> stmt) override;
    void visit_function_stmt(const std::shared_ptr<Function_stmt> stmt) override;
    void visit_return_stmt(const std::shared_ptr<Return_stmt> stmt) override;
    void visit_class_stmt(const std::shared_ptr<Class_stmt> stmt) override;

    // Resolve an expression.
    void resolve(std::shared_ptr<Expr> expr, int depth) { locals[expr] = depth; }

    // Start the interpreter run.
    void interpret(std::list<std::shared_ptr<Stmt>>& statements);
    // Get the result of the interpreter run.
    Literal get_result() { return result; }
};

#endif // __INTERPRETER_H
