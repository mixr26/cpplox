#ifndef __AST_PRINTER_H
#define __AST_PRINTER_H

#include "tree.h"

class Ast_printer : public Expr_visitor,
                    public std::enable_shared_from_this<Ast_printer> {
    std::string result;
public:
    Ast_printer() = default;
    Ast_printer(const Ast_printer&) = delete;
    Ast_printer(Ast_printer&&) = delete;
    ~Ast_printer() = default;
    Ast_printer& operator=(Ast_printer&) = delete;
    Ast_printer& operator=(Ast_printer&&) = delete;

    void visit_binary_expr(const std::shared_ptr<Binary_expr> expr) override;
    void visit_unary_expr(const std::shared_ptr<Unary_expr> expr) override;
    void visit_grouping_expr(const std::shared_ptr<Grouping_expr> expr) override;
    void visit_literal_expr(const std::shared_ptr<Literal_expr> expr) override;

    std::string&& print(std::shared_ptr<Expr> expr) {
        if (expr != nullptr)
            expr->accept(shared_from_this());
        return std::move(result);
    }
};

#endif // __AST_PRINTER_H
