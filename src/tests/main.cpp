
#include <unordered_map>
#include "binary_trees.h"
#include "blt/std/string.h"
#include "blt/profiling/profiler.h"
#include "blt/std/logging.h"
#include "blt/std/time.h"

int main() {
    binaryTreeTest();
    
    BLT_TRACE(10);
    BLT_TRACE(10.5);
    
    BLT_TRACE("Hello %d World!\n", 50);
    BLT_DEBUG("Hello %E World!\n", 1205200.0);
    BLT_INFO("Hello World!\n");
    BLT_WARN("Hello World!\n");
    BLT_ERROR("Hello World!\n");
    BLT_FATAL("Hello World!\n");
    
    std::string hello = "superSexyMax";
    std::cout << "String starts with: " << blt::string::contains(hello, "superSexyMaxE") << "\n";
    
    return 0;
}