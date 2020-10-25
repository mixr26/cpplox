#include <cassert>
#include <type_traits>
#include <iostream>

#include "interpreter.h"
#include "runtime_error.h"
#include "error_handling.h"

// Evaluate an expression. Just a wrapper around the call to accept method.
void Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    expr->accept(shared_from_this());
}

// Execute a statement. Just a wrapper around the call to accept method.
void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    stmt->accept(shared_from_this());
}

// Execute statements which compose a block.
void Interpreter::execute_block(std::list<std::shared_ptr<Stmt>>& statements,
                                std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = this->environment;

    this->environment = environment;
    for (auto statement : statements)
        execute(statement);

    this->environment = previous;
}

// Is the literal considered to be TRUE.
bool Interpreter::is_truthy() {
    bool ret = std::visit([](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr(std::is_same_v<T, bool>)
            return arg;
        else if constexpr(std::is_same_v<T, std::nullptr_t>)
            return false;
        else
            return true;
    }, result.value);

    return ret;
}

// Add two literals.
void Interpreter::add(auto& left) {
    std::visit([&result = result](auto& left, auto& right) {
        using T = std::decay_t<decltype(left)>;
        using U = std::decay_t<decltype(right)>;
        if constexpr(std::is_same_v<T, U> && (std::is_same_v<T, std::string>
                                              || std::is_same_v<T, double>))
            result.value = left + right;
        else throw std::runtime_error("Operands must be two numbers or two "
                                      "strings!");
    }, left, result.value);
}

// Are two literals equal.
bool Interpreter::is_equal(Literal& left) {
    bool ret = std::visit([](auto& left, auto& right) {
        using T = std::decay_t<decltype(left)>;
        using U = std::decay_t<decltype(right)>;
        if constexpr(std::is_same_v<T, U>)
            return left == right;
        else
            return false;
    }, left.value, result.value);

    return ret;
}

// Implementation of visitor interface.

// Interpret a single literal.
void Interpreter::visit_literal_expr(const std::shared_ptr<Literal_expr> expr) {
    auto token = expr->get_literal();
    assert(token->get_type() == Token_type::NIL
           || token->get_type() == Token_type::NUMBER
           || token->get_type() == Token_type::STRING
           || token->get_type() == Token_type::TRUE
           || token->get_type() == Token_type::FALSE);

    if (token->get_type() == Token_type::NIL)
        result.value = nullptr;
    else if (token->get_type() == Token_type::NUMBER)
        result.value = token->get_value();
    else if (token->get_type() == Token_type::STRING)
        result.value = token->get_lexeme();
    else if (token->get_type() == Token_type::TRUE)
        result.value = true;
    else
        result.value = false;
}

// Interpret a grouping expression.
void Interpreter::visit_grouping_expr(const std::shared_ptr<Grouping_expr> expr) {
    evaluate(expr->get_expr());
}

// Interpret a unary expression.
void Interpreter::visit_unary_expr(const std::shared_ptr<Unary_expr> expr) {
    evaluate(expr->get_right());

    assert(expr->get_op()->get_type() == Token_type::MINUS
           || expr->get_op()->get_type() == Token_type::BANG);

    switch (expr->get_op()->get_type()) {
    case Token_type::MINUS:
        result.value = -std::get<double>(result.value);
        break;
    case Token_type::BANG:
        result.value = !is_truthy();
        break;
    // Unreachable.
    default:
        break;
    }
    return;
}

// Interpret a binary expression.
void Interpreter::visit_binary_expr(const std::shared_ptr<Binary_expr> expr) {
    assert(expr->get_op()->get_type() == Token_type::MINUS
           || expr->get_op()->get_type() == Token_type::SLASH
           || expr->get_op()->get_type() == Token_type::STAR
           || expr->get_op()->get_type() == Token_type::PLUS
           || expr->get_op()->get_type() == Token_type::GREATER
           || expr->get_op()->get_type() == Token_type::GREATER_EQUAL
           || expr->get_op()->get_type() == Token_type::LESS
           || expr->get_op()->get_type() == Token_type::LESS_EQUAL
           || expr->get_op()->get_type() == Token_type::BANG_EQUAL
           || expr->get_op()->get_type() == Token_type::EQUAL_EQUAL);

    evaluate(expr->get_left());
    Literal left = result;
    evaluate(expr->get_right());

    try {
        switch(expr->get_op()->get_type()) {
        case Token_type::GREATER:
            result.value = std::get<double>(left.value)
                           > std::get<double>(result.value);
            break;
        case Token_type::GREATER_EQUAL:
            result.value = std::get<double>(left.value)
                           >= std::get<double>(result.value);
            break;
        case Token_type::LESS:
            result.value = std::get<double>(left.value)
                           < std::get<double>(result.value);
            break;
        case Token_type::LESS_EQUAL:
            result.value = std::get<double>(left.value)
                           <= std::get<double>(result.value);
            break;
        case Token_type::BANG_EQUAL:
            result.value = !is_equal(left);
            break;
        case Token_type::EQUAL_EQUAL:
            result.value = is_equal(left);
            break;
        case Token_type::MINUS:
            result.value = std::get<double>(left.value)
                           - std::get<double>(result.value);
            break;
        case Token_type::SLASH:
            result.value = std::get<double>(left.value)
                           / std::get<double>(result.value);
            break;
        case Token_type::STAR:
            result.value = std::get<double>(left.value)
                           * std::get<double>(result.value);
            break;
        case Token_type::PLUS:
        {
            add(left.value);
            break;
        }
        default:
            break;
        }
    } catch (std::bad_variant_access&) {
        throw Runtime_error("Operands must be numbers!", expr->get_op());
    } catch (std::runtime_error& e) {
        throw Runtime_error(e.what(), expr->get_op());
    }
}

// Interpret a variable use.
void Interpreter::visit_variable_expr(const std::shared_ptr<Variable_expr> expr) {
    result = environment->get(expr->get_name());
}

// Interpret a variable assignment.
void Interpreter::visit_assign_expr(const std::shared_ptr<Assign_expr> expr) {
    evaluate(expr->get_value());

    environment->assign(expr->get_name(), result);
}

// Interpret an expression statement.
void Interpreter::visit_expression_stmt(const std::shared_ptr<Expression_stmt> stmt) {
    evaluate(stmt->get_expr());
}

// Interpret a print statement.
void Interpreter::visit_print_stmt(const std::shared_ptr<Print_stmt> stmt) {
    evaluate(stmt->get_expr());
    std::cout << result << std::endl;
}

// Interpret a variable declaration.
void Interpreter::visit_var_stmt(const std::shared_ptr<Var_stmt> stmt) {
    Literal value;
    value.value = nullptr;
    if (stmt->get_initializer() != nullptr) {
        evaluate(stmt->get_initializer());
        value = result;
    }

    environment->define(stmt->get_name()->get_lexeme(), value);
}

// Interpret a block of statements.
void Interpreter::visit_block_stmt(const std::shared_ptr<Block_stmt> stmt) {
    execute_block(stmt->get_statements(),
                  std::make_shared<Environment>(environment));
}

// Start the interpreter run.
void Interpreter::interpret(std::list<std::shared_ptr<Stmt>>&& statements) {
    try {
        for (auto stmt : statements)
            execute(stmt);
    } catch (Runtime_error& e) {
        error_handling::error(e.get_token(), e.what());
    }
}
