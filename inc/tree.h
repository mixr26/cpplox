#ifndef __TREE_H
#define __TREE_H

#include <memory>

#include "token.h"

// Forward declarations.
class Binary_expr;
class Unary_expr;
class Grouping_expr;
class Literal_expr;

// The following classes describe expression nodes of the AST.

// Visitor class for expression nodes.
class Expr_visitor {
public:
    Expr_visitor() = default;
    Expr_visitor(const Expr_visitor&) = delete;
    Expr_visitor(Expr_visitor&&) = delete;
    ~Expr_visitor() = default;
    Expr_visitor& operator=(Expr_visitor&) = delete;
    Expr_visitor& operator=(Expr_visitor&&) = delete;

    virtual void visit_binary_expr(const std::shared_ptr<Binary_expr> expr) = 0;
    virtual void visit_unary_expr(const std::shared_ptr<Unary_expr> expr) = 0;
    virtual void visit_grouping_expr(const std::shared_ptr<Grouping_expr> expr) = 0;
    virtual void visit_literal_expr(const std::shared_ptr<Literal_expr> expr) = 0;
};

// Parent class for expression nodes.
class Expr {
public:
    Expr() = default;
    Expr(const Expr&) = default;
    Expr(Expr&&) = default;
    virtual ~Expr() = default;
    Expr& operator=(Expr&) = default;
    Expr& operator=(Expr&&) = default;

    // Pure virtual accept method of the visitor pattern.
    // Since virtual methods cannot be generic, this function will not return
    // anything, instead it will save the return value ins the visitor
    // object state.
    virtual void accept(const std::shared_ptr<Expr_visitor> visitor) = 0;
};

// Expression node describing binary operations.
class Binary_expr : public Expr,
                    public std::enable_shared_from_this<Binary_expr> {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::shared_ptr<Token> op;
public:
    Binary_expr(std::shared_ptr<Expr> left,
           std::shared_ptr<Expr> right,
           std::shared_ptr<Token> op)
        : Expr(), left(left), right(right), op(op) {}
    Binary_expr(const Binary_expr&) = default;
    Binary_expr(Binary_expr&&) = default;
    virtual ~Binary_expr() = default;
    Binary_expr& operator=(Binary_expr&) = default;
    Binary_expr& operator=(Binary_expr&&) = default;

    std::shared_ptr<Expr> get_left() { return left; }
    std::shared_ptr<Expr> get_right() { return right; }
    std::shared_ptr<Token> get_op() { return op; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_binary_expr(shared_from_this());
    }
};

// Expression node describing unary operations.
class Unary_expr : public Expr,
                   public std::enable_shared_from_this<Unary_expr> {
    std::shared_ptr<Expr> right;
    std::shared_ptr<Token> op;
public:
    Unary_expr(std::shared_ptr<Expr> right,
           std::shared_ptr<Token> op)
        : Expr(), right(right), op(op) {}
    Unary_expr(const Unary_expr&) = default;
    Unary_expr(Unary_expr&&) = default;
    virtual ~Unary_expr() = default;
    Unary_expr& operator=(Unary_expr&) = default;
    Unary_expr& operator=(Unary_expr&&) = default;

    std::shared_ptr<Expr> get_right() { return right; }
    std::shared_ptr<Token> get_op() { return op; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_unary_expr(shared_from_this());
    }
};

// Expression node describing the grouping of expressions (eg. (a + b)).
class Grouping_expr : public Expr,
                      public std::enable_shared_from_this<Grouping_expr> {
    std::shared_ptr<Expr> expr;
public:
    Grouping_expr(std::shared_ptr<Expr> expr)
        : Expr(), expr(expr) {}
    Grouping_expr(const Grouping_expr&) = default;
    Grouping_expr(Grouping_expr&&) = default;
    virtual ~Grouping_expr() = default;
    Grouping_expr& operator=(Grouping_expr&) = default;
    Grouping_expr& operator=(Grouping_expr&&) = default;

    std::shared_ptr<Expr> get_expr() { return expr; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_grouping_expr(shared_from_this());
    }
};

// Expression node describing a literal.
class Literal_expr : public Expr,
                     public std::enable_shared_from_this<Literal_expr> {
    std::shared_ptr<Token> literal;
public:
    Literal_expr(std::shared_ptr<Token> literal)
        : Expr(), literal(literal) {}
    Literal_expr(const Literal_expr&) = default;
    Literal_expr(Literal_expr&&) = default;
    virtual ~Literal_expr() = default;
    Literal_expr& operator=(Literal_expr&) = default;
    Literal_expr& operator=(Literal_expr&&) = default;

    std::shared_ptr<Token> get_literal() { return literal; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_literal_expr(shared_from_this());
    }
};

#endif // __TREE_H
