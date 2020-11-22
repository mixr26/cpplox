#ifndef __RESOLVER_H
#define __RESOLVER_H

#include <stack>
#include <unordered_map>

#include "tree.h"

class Interpreter;

// Visitor class which resolves all of the variables that the AST contains.
class Resolver : public Expr_visitor,
                 public Stmt_visitor,
                 public std::enable_shared_from_this<Resolver> {
    using scope = std::unordered_map<std::string, bool>;

    std::shared_ptr<Interpreter> interpreter;

    // Describes whether we are resolving something inside a function declaration.
    enum class Function_type {
        NONE,
        FUNCTION,
        INITIALIZER,
        METHOD
    };
    Function_type current_function = Function_type::NONE;

    // Describes whether we are resolving something inside a class declaration.
    enum class Class_type {
        NONE,
        CLASS
    };
    Class_type current_class = Class_type::NONE;

    // Stack of scopes.
    std::vector<scope> scopes;

    // Resolve a single statement.
    void resolve(std::shared_ptr<Stmt> stmt);
    // Resolve a single expression.
    void resolve(std::shared_ptr<Expr> expr);
    // Create a new block scope.
    void begin_scope() { scopes.push_back(scope()); }
    // Exit a block scope.
    void end_scope() { scopes.pop_back(); }
    // Declare a binding.
    void declare(std::shared_ptr<Token> name);
    // Define a binding.
    void define(std::shared_ptr<Token> name);
    // Resolve a local variable.
    void resolve_local(std::shared_ptr<Expr> expr, std::shared_ptr<Token> name);
    // Resolve a function.
    void resolve_function(std::shared_ptr<Function_stmt> function, Function_type type);
    // Resolve a lambda.
    void resolve_lambda(std::shared_ptr<Lambda_expr> lambda);
public:
    // Overridden visitor methods.
    void visit_block_stmt(std::shared_ptr<Block_stmt> stmt) override;
    void visit_var_stmt(std::shared_ptr<Var_stmt> stmt) override;
    void visit_function_stmt(std::shared_ptr<Function_stmt> stmt) override;
    void visit_expression_stmt(std::shared_ptr<Expression_stmt> stmt) override;
    void visit_if_stmt(std::shared_ptr<If_stmt> stmt) override;
    void visit_print_stmt(std::shared_ptr<Print_stmt> stmt) override;
    void visit_return_stmt(std::shared_ptr<Return_stmt> stmt) override;
    void visit_while_stmt(std::shared_ptr<While_stmt> stmt) override;
    void visit_class_stmt(std::shared_ptr<Class_stmt> stmt) override;

    void visit_variable_expr(std::shared_ptr<Variable_expr> expr) override;
    void visit_assign_expr(std::shared_ptr<Assign_expr> expr) override;
    void visit_binary_expr(std::shared_ptr<Binary_expr> expr) override;
    void visit_call_expr(std::shared_ptr<Call_expr> expr) override;
    void visit_grouping_expr(std::shared_ptr<Grouping_expr> expr) override;
    void visit_literal_expr(std::shared_ptr<Literal_expr> expr) override;
    void visit_logical_expr(std::shared_ptr<Logical_expr> expr) override;
    void visit_unary_expr(std::shared_ptr<Unary_expr> expr) override;
    void visit_lambda_expr(std::shared_ptr<Lambda_expr> expr) override;
    void visit_get_expr(std::shared_ptr<Get_expr> expr) override;
    void visit_set_expr(std::shared_ptr<Set_expr> expr) override;
    void visit_this_expr(std::shared_ptr<This_expr> expr) override;
    void visit_super_expr(std::shared_ptr<Super_expr> expr) override;

    // Resolve a list of statements.
    void resolve(std::list<std::shared_ptr<Stmt>>& statements);

    Resolver(std::shared_ptr<Interpreter> interpreter)
        : interpreter(interpreter) {}
    Resolver(const Resolver&) = delete;
    Resolver(Resolver&&) = delete;
    ~Resolver() = default;
    Resolver& operator=(Resolver&) = delete;
    Resolver& operator=(Resolver&&) = delete;
};

#endif // __RESOLVER_H
