#include <iostream>

extern void test_add();
extern void test_sub();

int main() {
    std::cout << "Running tests..." << std::endl;
    test_add();
    test_sub();
    std::cout << "Tests completed." << std::endl;
    return 0;
}
