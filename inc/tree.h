#ifndef __TREE_H
#define __TREE_H

#include "scanner.h"

// Forward declarations.
class Binary;
class Unary;
class Grouping;
class Literal;

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

    virtual void visit_binary_expr(const std::shared_ptr<Binary> expr) = 0;
    virtual void visit_unary_expr(const std::shared_ptr<Unary> expr) = 0;
    virtual void visit_grouping_expr(const std::shared_ptr<Grouping> expr) = 0;
    virtual void visit_literal_expr(const std::shared_ptr<Literal> expr) = 0;
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
class Binary : public Expr, public std::enable_shared_from_this<Binary> {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::shared_ptr<Token> op;
public:
    Binary(std::shared_ptr<Expr> left,
           std::shared_ptr<Expr> right,
           std::shared_ptr<Token> op)
        : Expr(), left(left), right(right), op(op) {}
    Binary(const Binary&) = default;
    Binary(Binary&&) = default;
    virtual ~Binary() = default;
    Binary& operator=(Binary&) = default;
    Binary& operator=(Binary&&) = default;

    std::shared_ptr<Expr> get_left() { return left; }
    std::shared_ptr<Expr> get_right() { return right; }
    std::shared_ptr<Token> get_op() { return op; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_binary_expr(shared_from_this());
    }
};

// Expression node describing unary operations.
class Unary : public Expr, public std::enable_shared_from_this<Unary> {
    std::shared_ptr<Expr> right;
    std::shared_ptr<Token> op;
public:
    Unary(std::shared_ptr<Expr> right,
           std::shared_ptr<Token> op)
        : Expr(), right(right), op(op) {}
    Unary(const Unary&) = default;
    Unary(Unary&&) = default;
    virtual ~Unary() = default;
    Unary& operator=(Unary&) = default;
    Unary& operator=(Unary&&) = default;

    std::shared_ptr<Expr> get_right() { return right; }
    std::shared_ptr<Token> get_op() { return op; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_unary_expr(shared_from_this());
    }
};

// Expression node describing the grouping of expressions (eg. (a + b)).
class Grouping : public Expr, public std::enable_shared_from_this<Grouping> {
    std::shared_ptr<Expr> expr;
public:
    Grouping(std::shared_ptr<Expr> expr)
        : Expr(), expr(expr) {}
    Grouping(const Grouping&) = default;
    Grouping(Grouping&&) = default;
    virtual ~Grouping() = default;
    Grouping& operator=(Grouping&) = default;
    Grouping& operator=(Grouping&&) = default;

    std::shared_ptr<Expr> get_expr() { return expr; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_grouping_expr(shared_from_this());
    }
};

// Expression node describing a literal.
class Literal : public Expr, public std::enable_shared_from_this<Literal> {
    std::shared_ptr<Token> literal;
public:
    Literal(std::shared_ptr<Token> literal)
        : Expr(), literal(literal) {}
    Literal(const Literal&) = default;
    Literal(Literal&&) = default;
    virtual ~Literal() = default;
    Literal& operator=(Literal&) = default;
    Literal& operator=(Literal&&) = default;

    std::shared_ptr<Token> get_literal() { return literal; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_literal_expr(shared_from_this());
    }
};

#endif // __TREE_H
