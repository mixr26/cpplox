#include "driver.h"
#include "scanner.h"

// Run the interpreter.
void run(std::string source) {
    Scanner scanner(source);

    std::list<Token>& tokens = scanner.scan_tokens();
    for (Token tok : tokens)
        std::cout << tok;
}
