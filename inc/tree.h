#ifndef __TREE_H
#define __TREE_H

#include <memory>
#include <list>

#include "token.h"

// Forward declarations.
class Binary_expr;
class Unary_expr;
class Grouping_expr;
class Literal_expr;
class Variable_expr;
class Assign_expr;

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
    virtual void visit_variable_expr(const std::shared_ptr<Variable_expr> expr) = 0;
    virtual void visit_assign_expr(const std::shared_ptr<Assign_expr> expr) = 0;
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

    virtual std::shared_ptr<Expr> make_assignment_expr(std::shared_ptr<Expr> left,
                                                       std::shared_ptr<Expr> right) { return nullptr; }
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

// Expression node describing a variable.
class Variable_expr : public Expr,
                      public std::enable_shared_from_this<Variable_expr> {
    std::shared_ptr<Token> name;
public:
    Variable_expr(std::shared_ptr<Token> name)
        : Expr(), name(name) {}
    Variable_expr(const Variable_expr&) = default;
    Variable_expr(Variable_expr&&) = default;
    virtual ~Variable_expr() = default;
    Variable_expr& operator=(Variable_expr&) = default;
    Variable_expr& operator=(Variable_expr&&) = default;

    std::shared_ptr<Token> get_name() { return name; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_variable_expr(shared_from_this());
    }

    std::shared_ptr<Expr> make_assignment_expr(std::shared_ptr<Expr> left,
                                               std::shared_ptr<Expr> right) {
        std::shared_ptr<Assign_expr> assign =
                std::make_shared<Assign_expr>(std::dynamic_pointer_cast<Variable_expr>(left)->get_name(),
                                              right);
        return std::static_pointer_cast<Expr>(assign);
    }
};

// Expression node describing an assignment.
class Assign_expr : public Expr,
                    public std::enable_shared_from_this<Assign_expr> {
    std::shared_ptr<Token> name;
    std::shared_ptr<Expr> value;
public:
    Assign_expr(std::shared_ptr<Token> name, std::shared_ptr<Expr> value)
        : Expr(), name(name), value(value) {}
    Assign_expr(const Assign_expr&) = default;
    Assign_expr(Assign_expr&&) = default;
    virtual ~Assign_expr() = default;
    Assign_expr& operator=(Assign_expr&) = default;
    Assign_expr& operator=(Assign_expr&&) = default;

    std::shared_ptr<Token> get_name() { return name; }
    std::shared_ptr<Expr> get_value() { return value; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_assign_expr(shared_from_this());
    }
};

// The following classes describe statement nodes of the AST.

// Forward declarations.
class Expression_stmt;
class Print_stmt;
class Var_stmt;
class Block_stmt;

// Visitor class for statement nodes.
class Stmt_visitor {
public:
    Stmt_visitor() = default;
    Stmt_visitor(const Stmt_visitor&) = delete;
    Stmt_visitor(Stmt_visitor&&) = delete;
    ~Stmt_visitor() = default;
    Stmt_visitor& operator=(Stmt_visitor&) = delete;
    Stmt_visitor& operator=(Stmt_visitor&&) = delete;

    virtual void visit_expression_stmt(const std::shared_ptr<Expression_stmt> stmt) = 0;
    virtual void visit_print_stmt(const std::shared_ptr<Print_stmt> stmt) = 0;
    virtual void visit_var_stmt(const std::shared_ptr<Var_stmt> stmt) = 0;
    virtual void visit_block_stmt(const std::shared_ptr<Block_stmt> stmt) = 0;
};

// Parent class for statement nodes
class Stmt {
public:
    Stmt() = default;
    Stmt(const Stmt&) = default;
    Stmt(Stmt&&) = default;
    virtual ~Stmt() = default;
    Stmt& operator=(Stmt&) = default;
    Stmt& operator=(Stmt&&) = default;

    // Pure virtual accept method of the visitor pattern.
    virtual void accept(const std::shared_ptr<Stmt_visitor> visitor) = 0;
};

// Statement node describing expression statements.
class Expression_stmt : public Stmt,
                        public std::enable_shared_from_this<Expression_stmt> {
    std::shared_ptr<Expr> expr;
public:
    Expression_stmt(std::shared_ptr<Expr> expr) : Stmt(), expr(expr) {}
    Expression_stmt(const Expression_stmt&) = default;
    Expression_stmt(Expression_stmt&&) = default;
    virtual ~Expression_stmt() = default;
    Expression_stmt& operator=(Expression_stmt&) = default;
    Expression_stmt& operator=(Expression_stmt&&) = default;

    std::shared_ptr<Expr> get_expr() { return expr; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_expression_stmt(shared_from_this());
    }
};

// Statement node describing the print statement.
class Print_stmt : public Stmt,
                   public std::enable_shared_from_this<Print_stmt> {
    std::shared_ptr<Expr> expr;
public:
    Print_stmt(std::shared_ptr<Expr> expr) : Stmt(), expr(expr) {}
    Print_stmt(const Print_stmt&) = default;
    Print_stmt(Print_stmt&&) = default;
    virtual ~Print_stmt() = default;
    Print_stmt& operator=(Print_stmt&) = default;
    Print_stmt& operator=(Print_stmt&&) = default;

    std::shared_ptr<Expr> get_expr() { return expr; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_print_stmt(shared_from_this());
    }
};

// Statement node describing the variable declaration statement.
class Var_stmt : public Stmt,
                 public std::enable_shared_from_this<Var_stmt> {
    std::shared_ptr<Token> name;
    std::shared_ptr<Expr> initializer;
public:
    Var_stmt(std::shared_ptr<Token> name,
             std::shared_ptr<Expr> initializer = nullptr)
        : Stmt(), name(name), initializer(initializer) {}
    Var_stmt(const Var_stmt&) = default;
    Var_stmt(Var_stmt&&) = default;
    virtual ~Var_stmt() = default;
    Var_stmt& operator=(Var_stmt&) = default;
    Var_stmt& operator=(Var_stmt&&) = default;

    std::shared_ptr<Expr> get_initializer() { return initializer; }
    std::shared_ptr<Token> get_name() { return name; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_var_stmt(shared_from_this());
    }
};

// Statement node describing a block of statements.
class Block_stmt : public Stmt,
                   public std::enable_shared_from_this<Block_stmt> {
    std::list<std::shared_ptr<Stmt>> statements;
public:
    Block_stmt(std::list<std::shared_ptr<Stmt>>&& statements)
        : statements(statements) {}
    Block_stmt(const Block_stmt&) = default;
    Block_stmt(Block_stmt&&) = default;
    virtual ~Block_stmt() = default;
    Block_stmt& operator=(Block_stmt&) = default;
    Block_stmt& operator=(Block_stmt&&) = default;

    std::list<std::shared_ptr<Stmt>>& get_statements() { return statements; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_block_stmt(shared_from_this());
    }
};


#endif // __TREE_H
