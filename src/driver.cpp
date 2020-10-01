#include "driver.h"
#include "scanner.h"
#include "tree.h"
#include "ast_printer.h"

// Run the interpreter.
void run(std::string source) {
    Scanner scanner(source);

    std::list<std::shared_ptr<Token>>& tokens = scanner.scan_tokens();
    for (auto tok : tokens)
        std::cout << *tok;
}

void test_printer() {
    std::shared_ptr<Literal> lit1 = std::make_shared<Literal>(
                std::make_shared<Token>(TokenType::NUMBER, "45.67", 0, 45.67));
    std::shared_ptr<Grouping> group = std::make_shared<Grouping>(lit1);
    std::shared_ptr<Token> mult
            = std::make_shared<Token>(TokenType::STAR, "*", 0);
    std::shared_ptr<Literal> lit2= std::make_shared<Literal>(
                std::make_shared<Token>(TokenType::NUMBER, "123", 0, 123));
    std::shared_ptr<Token> neg
            = std::make_shared<Token>(TokenType::MINUS, "-", 0);
    std::shared_ptr<Unary> un = std::make_shared<Unary>(lit2, neg);
    std::shared_ptr<Binary> exp = std::make_shared<Binary>(un, group, mult);

    std::shared_ptr<Ast_printer> printer = std::make_shared<Ast_printer>();
    std::cout << printer->print(exp);
}
