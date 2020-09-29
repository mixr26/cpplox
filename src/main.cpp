#include <iostream>

#include "driver.h"
#include "error_handling.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        error(0, "Source file not provided!");
        exit(1);
    }
    run(argv[1]);

    if (had_error)
        exit(1);

    return 0;
}
