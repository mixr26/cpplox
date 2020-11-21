#include "resolver.h"
#include "error_handling.h"
#include "interpreter.h"

// Resolve a list of statements.
void Resolver::resolve(std::list<std::shared_ptr<Stmt>>& statements) {
    for (auto stmt : statements)
        resolve(stmt);
}

// Resolve a single statement.
void Resolver::resolve(std::shared_ptr<Stmt> stmt) {
    stmt->accept(shared_from_this());
}

// Resolve a single expression.
void Resolver::resolve(std::shared_ptr<Expr> expr) {
    expr->accept(shared_from_this());
}

// Declare a binding.
void Resolver::declare(std::shared_ptr<Token> name) {
    if (scopes.empty())
        return;

    scope& in_scope = scopes.back();
    if (in_scope.find(name->get_lexeme()) != in_scope.end())
        error_handling::error(name, "Already a variable with this name in"
                              " this scope!");

    in_scope[name->get_lexeme()] = false;
}

// Define a binding.
void Resolver::define(std::shared_ptr<Token> name) {
    if (scopes.empty())
        return;

    scope& in_scope = scopes.back();
    in_scope[name->get_lexeme()] = true;
}

// Resolve a local variable.
void Resolver::resolve_local(std::shared_ptr<Expr> expr,
                             std::shared_ptr<Token> name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes.at(i).find(name->get_lexeme()) != scopes.at(i).end()) {
            interpreter->resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

// Resolve a function.
void Resolver::resolve_function(std::shared_ptr<Function_stmt> function,
                                Function_type type) {
    Function_type enclosing_function = current_function;
    current_function = type;

    begin_scope();
    for (auto param : function->get_params()) {
        declare(param);
        define(param);
    }
    resolve(function->get_body());
    end_scope();
    current_function = enclosing_function;
}

// Resolve a lambda.
void Resolver::resolve_lambda(std::shared_ptr<Lambda_expr> lambda) {
    begin_scope();
    for (auto param : lambda->get_params()) {
        declare(param);
        define(param);
    }
    resolve(lambda->get_body());
    end_scope();
}

// Overridden visitor methods.

void Resolver::visit_block_stmt(std::shared_ptr<Block_stmt> stmt) {
    begin_scope();
    resolve(stmt->get_statements());
    end_scope();
}

void Resolver::visit_var_stmt(std::shared_ptr<Var_stmt> stmt) {
    declare(stmt->get_name());
    if (stmt->get_initializer() != nullptr)
        resolve(stmt->get_initializer());
    define(stmt->get_name());
}

void Resolver::visit_function_stmt(std::shared_ptr<Function_stmt> stmt) {
    declare(stmt->get_name());
    define(stmt->get_name());

    resolve_function(stmt, Function_type::FUNCTION);
}

void Resolver::visit_expression_stmt(std::shared_ptr<Expression_stmt> stmt) {
    resolve(stmt->get_expr());
}

void Resolver::visit_if_stmt(std::shared_ptr<If_stmt> stmt) {
    resolve(stmt->get_condition());
    resolve(stmt->get_then_branch());
    if (stmt->get_else_branch() != nullptr)
        resolve(stmt->get_else_branch());
}

void Resolver::visit_print_stmt(std::shared_ptr<Print_stmt> stmt) {
    resolve(stmt->get_expr());
}

void Resolver::visit_return_stmt(std::shared_ptr<Return_stmt> stmt) {
    if (current_function == Function_type::NONE)
        error_handling::error(stmt->get_keyword(),
                              "Can't return from top-level code!");

    if (stmt->get_value() != nullptr) {
        if (current_function == Function_type::INITIALIZER)
            error_handling::error(stmt->get_keyword(),
                                  "Can't return a value from an initializer!");
        resolve(stmt->get_value());
    }
}

void Resolver::visit_while_stmt(std::shared_ptr<While_stmt> stmt) {
    resolve(stmt->get_condition());
    resolve(stmt->get_body());
}

void Resolver::visit_class_stmt(std::shared_ptr<Class_stmt> stmt) {
    Class_type enclosing_class = current_class;
    current_class = Class_type::CLASS;

    declare(stmt->get_name());
    define(stmt->get_name());

    begin_scope();
    auto& top_scope = scopes.back();
    top_scope["this"] = true;

    for (auto method : stmt->get_methods()) {
        Function_type declaration = Function_type::METHOD;
        if (method->get_name()->get_lexeme() == "init")
            declaration = Function_type::INITIALIZER;
        resolve_function(method, declaration);
    }

    end_scope();

    current_class = enclosing_class;
}

void Resolver::visit_variable_expr(std::shared_ptr<Variable_expr> expr) {
    if (!scopes.empty()
            && scopes.back().find(expr->get_name()->get_lexeme()) != scopes.back().end()
            && scopes.back()[expr->get_name()->get_lexeme()] == false)
        error_handling::error(expr->get_name(), "Can't read local variable"
                              " in its own initializer!");

    resolve_local(expr, expr->get_name());
}

void Resolver::visit_assign_expr(std::shared_ptr<Assign_expr> expr) {
    resolve(expr->get_value());
    resolve_local(expr, expr->get_name());
}

void Resolver::visit_binary_expr(std::shared_ptr<Binary_expr> expr) {
    resolve(expr->get_left());
    resolve(expr->get_right());
}

void Resolver::visit_call_expr(std::shared_ptr<Call_expr> expr) {
    resolve(expr->get_callee());

    for (auto argument : expr->get_arguments())
        resolve(argument);
}

void Resolver::visit_grouping_expr(std::shared_ptr<Grouping_expr> expr) {
    resolve(expr->get_expr());
}

void Resolver::visit_literal_expr(std::shared_ptr<Literal_expr> expr) {
    return;
}

void Resolver::visit_logical_expr(std::shared_ptr<Logical_expr> expr) {
    resolve(expr->get_left());
    resolve(expr->get_right());
}

void Resolver::visit_unary_expr(std::shared_ptr<Unary_expr> expr) {
    resolve(expr->get_right());
}

void Resolver::visit_lambda_expr(std::shared_ptr<Lambda_expr> expr) {
    resolve_lambda(expr);
}

void Resolver::visit_get_expr(std::shared_ptr<Get_expr> expr) {
    resolve(expr->get_object());
}

void Resolver::visit_set_expr(std::shared_ptr<Set_expr> expr) {
    resolve(expr->get_value());
    resolve(expr->get_object());
}

void Resolver::visit_this_expr(std::shared_ptr<This_expr> expr) {
    if (current_class == Class_type::NONE)
        error_handling::error(expr->get_keyword(),
                              "Can't use 'this' outside of a class!");

    resolve_local(expr, expr->get_keyword());
}
