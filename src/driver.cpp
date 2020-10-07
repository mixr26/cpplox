#include "driver.h"
#include "scanner.h"
#include "tree.h"
#include "ast_printer.h"
#include "parser.h"
#include "error_handling.h"

// Run the interpreter.
void run(std::string source) {
    Scanner scanner(source);
    Parser parser(scanner.scan_tokens());

    if (error_handling::had_error)
        return;

    std::shared_ptr<Ast_printer> printer = std::make_shared<Ast_printer>();
    std::cout << printer->print(parser.parse());
}

void test_printer() {
    std::shared_ptr<Literal_expr> lit1 = std::make_shared<Literal_expr>(
                std::make_shared<Token>(Token_type::NUMBER, "45.67", 0, 45.67));
    std::shared_ptr<Grouping_expr> group = std::make_shared<Grouping_expr>(lit1);
    std::shared_ptr<Token> mult
            = std::make_shared<Token>(Token_type::STAR, "*", 0);
    std::shared_ptr<Literal_expr> lit2= std::make_shared<Literal_expr>(
                std::make_shared<Token>(Token_type::NUMBER, "123", 0, 123));
    std::shared_ptr<Token> neg
            = std::make_shared<Token>(Token_type::MINUS, "-", 0);
    std::shared_ptr<Unary_expr> un = std::make_shared<Unary_expr>(lit2, neg);
    std::shared_ptr<Binary_expr> exp = std::make_shared<Binary_expr>(un, group, mult);

    std::shared_ptr<Ast_printer> printer = std::make_shared<Ast_printer>();
    std::cout << printer->print(exp);
}
