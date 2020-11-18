#include "driver.h"
#include "scanner.h"
#include "tree.h"
#include "ast_printer.h"
#include "parser.h"
#include "interpreter.h"
#include "resolver.h"
#include "error_handling.h"

// Run the interpreter.
void run(std::string source) {
    Scanner scanner(source);
    Parser parser(scanner.scan_tokens());
    std::list<std::shared_ptr<Stmt>>& statements = parser.parse();

    if (error_handling::had_error)
        return;

    std::shared_ptr<Interpreter> interpreter = std::make_shared<Interpreter>();
    std::shared_ptr<Resolver> resolver = std::make_shared<Resolver>(interpreter);
    resolver->resolve(statements);

    if (error_handling::had_error)
        return;

    interpreter->interpret(statements);
}
