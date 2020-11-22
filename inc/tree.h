#ifndef __TREE_H
#define __TREE_H

#include <memory>
#include <list>
#include <vector>

#include "token.h"

// Forward declarations.
class Binary_expr;
class Logical_expr;
class Unary_expr;
class Grouping_expr;
class Literal_expr;
class Variable_expr;
class Assign_expr;
class Call_expr;
class Lambda_expr;
class Get_expr;
class Set_expr;
class Super_expr;
class This_expr;

class Stmt;

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
    virtual void visit_logical_expr(const std::shared_ptr<Logical_expr> expr) = 0;
    virtual void visit_call_expr(const std::shared_ptr<Call_expr> expr) = 0;
    virtual void visit_lambda_expr(const std::shared_ptr<Lambda_expr> expr) = 0;
    virtual void visit_get_expr(const std::shared_ptr<Get_expr> expr) = 0;
    virtual void visit_set_expr(const std::shared_ptr<Set_expr> expr) = 0;
    virtual void visit_this_expr(const std::shared_ptr<This_expr> expr) = 0;
    virtual void visit_super_expr(const std::shared_ptr<Super_expr> expr) = 0;
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

    void accept(const std::shared_ptr<Expr_visitor> visitor) override {
        visitor->visit_binary_expr(shared_from_this());
    }
};

// Expression node describing a function call.
class Call_expr : public Expr,
                  public std::enable_shared_from_this<Call_expr> {
    std::shared_ptr<Expr> callee;
    std::shared_ptr<Token> paren;
    std::list<std::shared_ptr<Expr>> arguments;
public:
    Call_expr(std::shared_ptr<Expr> callee,
              std::shared_ptr<Token> paren,
              std::list<std::shared_ptr<Expr>>&& arguments)
        : Expr(), callee(callee), paren(paren), arguments(arguments) {}
    Call_expr(const Call_expr&) = default;
    Call_expr(Call_expr&&) = default;
    virtual ~Call_expr() = default;
    Call_expr& operator=(Call_expr&) = default;
    Call_expr& operator=(Call_expr&&) = default;

    std::shared_ptr<Expr> get_callee() { return callee; }
    std::shared_ptr<Token> get_paren() { return paren; }
    std::list<std::shared_ptr<Expr>>& get_arguments() { return arguments; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_call_expr(shared_from_this());
    }
};

// Expression node describing a class getter.
class Get_expr : public Expr,
                 public std::enable_shared_from_this<Get_expr> {
    std::shared_ptr<Expr> object;
    std::shared_ptr<Token> name;
public:
    Get_expr(std::shared_ptr<Expr> object,
              std::shared_ptr<Token> name)
        : Expr(), object(object), name(name) {}
    Get_expr(const Get_expr&) = default;
    Get_expr(Get_expr&&) = default;
    virtual ~Get_expr() = default;
    Get_expr& operator=(Get_expr&) = default;
    Get_expr& operator=(Get_expr&&) = default;

    std::shared_ptr<Expr> get_object() { return object; }
    std::shared_ptr<Token> get_name() { return name; }

    std::shared_ptr<Expr> make_assignment_expr(std::shared_ptr<Expr> left,
                                               std::shared_ptr<Expr> right) {
        std::shared_ptr<Set_expr> set =
                std::make_shared<Set_expr>(std::dynamic_pointer_cast<Get_expr>(left)->get_object(),
                                           std::dynamic_pointer_cast<Get_expr>(left)->get_name(),
                                           right);
        return std::static_pointer_cast<Expr>(set);
    }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_get_expr(shared_from_this());
    }
};

// Expression node describing a class setter.
class Set_expr : public Expr,
                 public std::enable_shared_from_this<Set_expr> {
    std::shared_ptr<Expr> object;
    std::shared_ptr<Token> name;
    std::shared_ptr<Expr> value;
public:
    Set_expr(std::shared_ptr<Expr> object,
             std::shared_ptr<Token> name,
             std::shared_ptr<Expr> value)
        : Expr(), object(object), name(name), value(value) {}
    Set_expr(const Set_expr&) = default;
    Set_expr(Set_expr&&) = default;
    virtual ~Set_expr() = default;
    Set_expr& operator=(Set_expr&) = default;
    Set_expr& operator=(Set_expr&&) = default;

    std::shared_ptr<Expr> get_object() { return object; }
    std::shared_ptr<Token> get_name() { return name; }
    std::shared_ptr<Expr> get_value() { return value; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_set_expr(shared_from_this());
    }
};

// Expression node describing a class THIS expression.
class This_expr : public Expr,
                  public std::enable_shared_from_this<This_expr> {
    std::shared_ptr<Token> keyword;
public:
    This_expr(std::shared_ptr<Token> keyword)
        : Expr(), keyword(keyword) {}
    This_expr(const This_expr&) = default;
    This_expr(This_expr&&) = default;
    virtual ~This_expr() = default;
    This_expr& operator=(This_expr&) = default;
    This_expr& operator=(This_expr&&) = default;

    std::shared_ptr<Token> get_keyword() { return keyword; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_this_expr(shared_from_this());
    }
};

// Expression node describing a class SUPER expression.
class Super_expr : public Expr,
                   public std::enable_shared_from_this<Super_expr> {
    std::shared_ptr<Token> keyword;
    std::shared_ptr<Token> method;
public:
    Super_expr(std::shared_ptr<Token> keyword, std::shared_ptr<Token> method)
        : Expr(), keyword(keyword), method(method) {}
    Super_expr(const Super_expr&) = default;
    Super_expr(Super_expr&&) = default;
    virtual ~Super_expr() = default;
    Super_expr& operator=(Super_expr&) = default;
    Super_expr& operator=(Super_expr&&) = default;

    std::shared_ptr<Token> get_keyword() { return keyword; }
    std::shared_ptr<Token> get_method() { return method; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_super_expr(shared_from_this());
    }
};

// Expression node describing logical operations.
class Logical_expr : public Expr,
                     public std::enable_shared_from_this<Logical_expr> {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::shared_ptr<Token> op;
public:
    Logical_expr(std::shared_ptr<Expr> left,
           std::shared_ptr<Expr> right,
           std::shared_ptr<Token> op)
        : Expr(), left(left), right(right), op(op) {}
    Logical_expr(const Logical_expr&) = default;
    Logical_expr(Logical_expr&&) = default;
    virtual ~Logical_expr() = default;
    Logical_expr& operator=(Logical_expr&) = default;
    Logical_expr& operator=(Logical_expr&&) = default;

    std::shared_ptr<Expr> get_left() { return left; }
    std::shared_ptr<Expr> get_right() { return right; }
    std::shared_ptr<Token> get_op() { return op; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override  {
        visitor->visit_logical_expr(shared_from_this());
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

// Expression node describing a lambda expression.
class Lambda_expr : public Expr,
                    public std::enable_shared_from_this<Lambda_expr> {
    std::vector<std::shared_ptr<Token>> params;
    std::list<std::shared_ptr<Stmt>> body;
public:
    Lambda_expr(std::vector<std::shared_ptr<Token>>&& params,
           std::list<std::shared_ptr<Stmt>>&& body)
        : Expr(), params(params), body(body) {}
    Lambda_expr(const Lambda_expr&) = default;
    Lambda_expr(Lambda_expr&&) = default;
    virtual ~Lambda_expr() = default;
    Lambda_expr& operator=(Lambda_expr&) = default;
    Lambda_expr& operator=(Lambda_expr&&) = default;

    std::vector<std::shared_ptr<Token>>& get_params() { return params; }
    std::list<std::shared_ptr<Stmt>>& get_body() { return body; }

    void accept(const std::shared_ptr<Expr_visitor> visitor) override {
        visitor->visit_lambda_expr(shared_from_this());
    }
};

// The following classes describe statement nodes of the AST.

// Forward declarations.
class Expression_stmt;
class Print_stmt;
class Var_stmt;
class Block_stmt;
class If_stmt;
class While_stmt;
class Function_stmt;
class Return_stmt;
class Class_stmt;

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
    virtual void visit_if_stmt(const std::shared_ptr<If_stmt> stmt) = 0;
    virtual void visit_while_stmt(const std::shared_ptr<While_stmt> stmt) = 0;
    virtual void visit_function_stmt(const std::shared_ptr<Function_stmt> stmt) = 0;
    virtual void visit_return_stmt(const std::shared_ptr<Return_stmt> stmt) = 0;
    virtual void visit_class_stmt(const std::shared_ptr<Class_stmt> stmt) = 0;
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

// Expression node describing a function declaration.
class Function_stmt : public Stmt,
                      public std::enable_shared_from_this<Function_stmt> {
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Token>> params;
    std::list<std::shared_ptr<Stmt>> body;
public:
    Function_stmt(std::shared_ptr<Token> name,
           std::vector<std::shared_ptr<Token>>&& params,
           std::list<std::shared_ptr<Stmt>>&& body)
        : Stmt(), name(name), params(params), body(body) {}
    Function_stmt(const Function_stmt&) = default;
    Function_stmt(Function_stmt&&) = default;
    virtual ~Function_stmt() = default;
    Function_stmt& operator=(Function_stmt&) = default;
    Function_stmt& operator=(Function_stmt&&) = default;

    std::shared_ptr<Token> get_name() { return name; }
    std::vector<std::shared_ptr<Token>>& get_params() { return params; }
    std::list<std::shared_ptr<Stmt>>& get_body() { return body; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override {
        visitor->visit_function_stmt(shared_from_this());
    }
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

// Statement node describing an 'if' statement.
class If_stmt : public Stmt,
                public std::enable_shared_from_this<If_stmt> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> then_branch;
    std::shared_ptr<Stmt> else_branch;
public:
    If_stmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch,
            std::shared_ptr<Stmt> else_branch)
        : condition(condition), then_branch(then_branch),
          else_branch(else_branch) {}
    If_stmt(const If_stmt&) = default;
    If_stmt(If_stmt&&) = default;
    virtual ~If_stmt() = default;
    If_stmt& operator=(If_stmt&) = default;
    If_stmt& operator=(If_stmt&&) = default;

    std::shared_ptr<Expr> get_condition() { return condition; }
    std::shared_ptr<Stmt> get_then_branch() { return then_branch; }
    std::shared_ptr<Stmt> get_else_branch() { return else_branch; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_if_stmt(shared_from_this());
    }
};

// Statement node describing a while loop.
class While_stmt : public Stmt,
                   public std::enable_shared_from_this<While_stmt> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
public:
    While_stmt(std::shared_ptr<Expr> condition,
            std::shared_ptr<Stmt> body)
        : condition(condition), body(body) {}
    While_stmt(const While_stmt&) = default;
    While_stmt(While_stmt&&) = default;
    virtual ~While_stmt() = default;
    While_stmt& operator=(While_stmt&) = default;
    While_stmt& operator=(While_stmt&&) = default;

    std::shared_ptr<Expr> get_condition() { return condition; }
    std::shared_ptr<Stmt> get_body() { return body; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_while_stmt(shared_from_this());
    }
};


// Statement node describing the return statement.
class Return_stmt : public Stmt,
                    public std::enable_shared_from_this<Return_stmt> {
    std::shared_ptr<Token> keyword;
    std::shared_ptr<Expr> value;
public:
    Return_stmt(std::shared_ptr<Token> keyword,
             std::shared_ptr<Expr> value)
        : Stmt(), keyword(keyword), value(value) {}
    Return_stmt(const Return_stmt&) = default;
    Return_stmt(Return_stmt&&) = default;
    virtual ~Return_stmt() = default;
    Return_stmt& operator=(Return_stmt&) = default;
    Return_stmt& operator=(Return_stmt&&) = default;

    std::shared_ptr<Expr> get_value() { return value; }
    std::shared_ptr<Token> get_keyword() { return keyword; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_return_stmt(shared_from_this());
    }
};

// Statement node describing a class declaration.
class Class_stmt : public Stmt,
                   public std::enable_shared_from_this<Class_stmt> {
    std::shared_ptr<Token> name;
    std::shared_ptr<Variable_expr> superclass;
    std::list<std::shared_ptr<Function_stmt>> methods;
public:
    Class_stmt(std::shared_ptr<Token> name,
               std::shared_ptr<Variable_expr> superclass,
               std::list<std::shared_ptr<Function_stmt>>&& methods)
        : Stmt(), name(name), superclass(superclass), methods(methods) {}
    Class_stmt(const Class_stmt&) = default;
    Class_stmt(Class_stmt&&) = default;
    virtual ~Class_stmt() = default;
    Class_stmt& operator=(Class_stmt&) = default;
    Class_stmt& operator=(Class_stmt&&) = default;

    std::shared_ptr<Token> get_name() { return name; }
    std::shared_ptr<Variable_expr> get_superclass() { return superclass; }
    std::list<std::shared_ptr<Function_stmt>>& get_methods() { return methods; }

    void accept(const std::shared_ptr<Stmt_visitor> visitor) override  {
        visitor->visit_class_stmt(shared_from_this());
    }
};

#endif // __TREE_H
