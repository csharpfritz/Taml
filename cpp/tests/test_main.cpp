#include <iostream>
#include "Taml.h"

#ifndef TAML_VERSION_MAJOR
#error "TAML_VERSION_MAJOR not defined"
#endif

int main() {
    static_assert(TAML_VERSION_MAJOR == 1, "Unexpected major version");
    std::cout << "TAML version: " << Taml::version() << std::endl;
    return 0;
}
