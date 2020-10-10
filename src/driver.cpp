#include "driver.h"
#include "scanner.h"
#include "tree.h"
#include "ast_printer.h"
#include "parser.h"
#include "interpreter.h"
#include "error_handling.h"

// Run the interpreter.
void run(std::string source) {
    Scanner scanner(source);
    Parser parser(scanner.scan_tokens());
    std::shared_ptr<Interpreter> interpreter = std::make_shared<Interpreter>();

    interpreter->interpret(parser.parse());

    if (error_handling::had_error)
        return;

    std::cout << interpreter->get_result() << std::endl;

    //std::shared_ptr<Ast_printer> printer = std::make_shared<Ast_printer>();
    //std::cout << printer->print(parser.parse());
}
