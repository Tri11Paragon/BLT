
#include "binary_trees.h"
#include "blt/std/string.h"

int main() {
    binaryTreeTest();
    
    std::string hello = "superSexyMax";
    std::cout << "String starts with: " << blt::String::contains(hello, "superSexyMaxE") << "\n";
}