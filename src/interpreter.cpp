#include <cassert>
#include <type_traits>
#include <iostream>
#include <string>
#include <chrono>

#include "class.h"
#include "instance.h"
#include "interpreter.h"
#include "runtime_error.h"
#include "error_handling.h"
#include "function.h"
#include "return.h"
#include "lambda.h"

Interpreter::Interpreter() : result(), locals() {
    class Clock_function : public Callable {
        Literal call(std::shared_ptr<Interpreter> interpreter,
                     std::vector<Literal> &arguments) override {
            Literal ret;
            ret.value = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(
                                            std::chrono::system_clock::now().time_since_epoch()).count());

            return ret;
        }

        uint32_t arity() override {
            return 0;
        }
    };

    Literal clock;
    clock.value = std::make_shared<Clock_function>();
    globals->define("clock", clock);
}

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
    try {
        this->environment = environment;
        for (auto statement : statements)
            execute(statement);
        this->environment = previous;
    } catch (Return return_value) {
        this->environment = previous;
        throw return_value;
    }
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

// Get the Callable class (and its children) instance.
std::shared_ptr<Callable> Interpreter::get_callable(Literal& callee,
                                                    std::shared_ptr<Token> parent) {
    return std::visit([&parent](auto& callee_value) {
        using T = std::decay_t<decltype(callee_value)>;
        if constexpr(std::is_same_v<T, std::shared_ptr<Callable>>)
            return callee_value;
        else if constexpr(std::is_same_v<T, std::shared_ptr<Function>>
                          || std::is_same_v<T, std::shared_ptr<Lambda>>
                          || std::is_same_v<T, std::shared_ptr<Class>>)
            return std::dynamic_pointer_cast<Callable>(callee_value);
        else {
            throw Runtime_error("Can call only functions and classes!",
                                parent);
            return std::dynamic_pointer_cast<Callable>(std::make_shared<Function>());
        }
    }, callee.value);
}

// Get the Instance class.
std::shared_ptr<Instance> Interpreter::get_instance(Literal& callee,
                                                    std::shared_ptr<Token> parent) {
    return std::visit([&parent](auto& callee_value) {
        using T = std::decay_t<decltype(callee_value)>;
        if constexpr(std::is_same_v<T, std::shared_ptr<Instance>>)
            return callee_value;
        else {
            throw Runtime_error("Only instances have fields!", parent);
            return std::make_shared<Instance>();
        }
    }, callee.value);
}

Literal Interpreter::look_up_variable(std::shared_ptr<Token> name,
                                      std::shared_ptr<Expr> expr) {
    if (locals.find(expr) != locals.end()) {
        int distance = locals[expr];
        return environment->get_at(distance, name->get_lexeme());
    }

    return globals->get(name);
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

    if (locals.find(expr) != locals.end()) {
        environment->assign_at(locals[expr], expr->get_name(), result);
    } else {
        globals->assign(expr->get_name(), result);
    }
}

// Interpret a logical expression.
void Interpreter::visit_logical_expr(const std::shared_ptr<Logical_expr> expr) {
    evaluate(expr->get_left());

    if (expr->get_op()->get_type() == Token_type::OR) {
        if (is_truthy())
            return;
    } else {
        if (!is_truthy())
            return;
    }

    evaluate(expr->get_right());
}

// Interpret a function call.
void Interpreter::visit_call_expr(const std::shared_ptr<Call_expr> expr) {
    evaluate(expr->get_callee());
    std::shared_ptr<Callable> callee = get_callable(result, expr->get_paren());

    std::vector<Literal> arguments;
    for (std::shared_ptr<Expr> arg : expr->get_arguments()) {
        evaluate(arg);
        arguments.push_back(result);
    }

    if (arguments.size() != callee->arity())
        throw Runtime_error("Expected " + std::to_string(callee->arity())
                            + " arguments, but got "
                            + std::to_string(arguments.size()) + "!",
                            expr->get_paren());

    result = callee->call(shared_from_this(), arguments);
}

// Interpret a lambda function.
void Interpreter::visit_lambda_expr(const std::shared_ptr<Lambda_expr> expr) {
    result.value = std::make_shared<Lambda>(expr, environment);
}

// Interpret a class object get expression.
void Interpreter::visit_get_expr(const std::shared_ptr<Get_expr> expr) {
    evaluate(expr->get_object());

    try {
        std::shared_ptr<Instance> object
                = std::get<std::shared_ptr<Instance>>(result.value);
        result = object->get(expr->get_name());
    } catch (std::bad_variant_access) {
        throw Runtime_error("Only instances have properties!",
                            expr->get_name());
    }
}

// Interpret a class object set expression.
void Interpreter::visit_set_expr(const std::shared_ptr<Set_expr> expr) {
    evaluate(expr->get_object());
    std::shared_ptr<Instance> object = get_instance(result, expr->get_name());

    evaluate(expr->get_value());
    object->set(expr->get_name(), result);
}

// Interpret a this expression
void Interpreter::visit_this_expr(const std::shared_ptr<This_expr> expr) {
    result = look_up_variable(expr->get_keyword(), expr);
}

// Interpret a function declaration.
void Interpreter::visit_function_stmt(const std::shared_ptr<Function_stmt> stmt) {
    Literal function;
    function.value = std::make_shared<Function>(stmt, environment, false);
    environment->define(stmt->get_name()->get_lexeme(), function);
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

// Interpret an if statement.
void Interpreter::visit_if_stmt(const std::shared_ptr<If_stmt> stmt) {
    evaluate(stmt->get_condition());
    if (is_truthy())
        execute(stmt->get_then_branch());
    else if (stmt->get_else_branch() != nullptr)
        execute(stmt->get_else_branch());
}

// Interpret a while loop.
void Interpreter::visit_while_stmt(const std::shared_ptr<While_stmt> stmt) {
    evaluate(stmt->get_condition());
    while (is_truthy()) {
        execute(stmt->get_body());
        evaluate(stmt->get_condition());
    }
}

// Interpret a return statement.
void Interpreter::visit_return_stmt(const std::shared_ptr<Return_stmt> stmt) {
    Literal value;
    value.value = nullptr;

    if (stmt->get_value()) {
        evaluate(stmt->get_value());
        value = result;
    }

    throw Return(result);
}

// Interpret a class declaration.
void Interpreter::visit_class_stmt(const std::shared_ptr<Class_stmt> stmt) {
    Literal temp;
    temp.value = nullptr;
    environment->define(stmt->get_name()->get_lexeme(), temp);

    Class::method_map methods;
    for (auto method : stmt->get_methods()) {
        std::shared_ptr<Function> function
                = std::make_shared<Function>(method, environment,
                                             method->get_name()->get_lexeme() == "init");
        methods[method->get_name()->get_lexeme()] = function;
    }

    std::shared_ptr<Class> klass
            = std::make_shared<Class>(stmt->get_name()->get_lexeme(), methods);
    temp.value = klass;
    environment->assign(stmt->get_name(), temp);
}

// Start the interpreter run.
void Interpreter::interpret(std::list<std::shared_ptr<Stmt>>& statements) {
    try {
        for (auto stmt : statements)
            execute(stmt);
    } catch (Runtime_error& e) {
        error_handling::error(e.get_token(), e.what());
    }
}
