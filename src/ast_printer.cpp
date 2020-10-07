#include <cassert>
#include <sstream>
#include <memory>

#include "ast_printer.h"

void Ast_printer::visit_binary_expr(const std::shared_ptr<Binary_expr> expr) {
    // Print the operation.
    result += "(" + expr->get_op()->get_lexeme();

    // Print the left operand.
    result += " ";
    expr->get_left()->accept(shared_from_this());

    // Print the right operand.
    result += " ";
    expr->get_right()->accept(shared_from_this());

    result += ")";
}

void Ast_printer::visit_unary_expr(const std::shared_ptr<Unary_expr> expr) {
    // Print the operation.
    result += "(" + expr->get_op()->get_lexeme();

    // Print the operand.
    result += " ";
    expr->get_right()->accept(shared_from_this());

    result += ")";
}

void Ast_printer::visit_grouping_expr(const std::shared_ptr<Grouping_expr> expr) {
    // Print the operation.
    result += "(group";

    // Print the operand.
    result += " ";
    expr->get_expr()->accept(shared_from_this());

    result += ")";
}

void Ast_printer::visit_literal_expr(const std::shared_ptr<Literal_expr> expr) {
    Token_type token_type = expr->get_literal()->get_type();
    assert(token_type == Token_type::NIL
           || token_type == Token_type::STRING
           || token_type == Token_type::NUMBER);

    if (token_type == Token_type::NIL)
        result += "nil";
    else if (token_type == Token_type::STRING
             || token_type == Token_type::NUMBER) {
        result += expr->get_literal()->get_lexeme();
    }
}
