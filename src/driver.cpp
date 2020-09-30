#include "driver.h"
#include "scanner.h"

// Run the interpreter.
void run(std::string source) {
    Scanner scanner(source);

    std::list<std::shared_ptr<Token>>& tokens = scanner.scan_tokens();
    for (auto tok : tokens)
        std::cout << *tok;
}
