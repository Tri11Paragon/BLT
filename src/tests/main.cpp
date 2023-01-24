
#include <unordered_map>
#include "binary_trees.h"
#include "blt/std/string.h"
#include "blt/profiling/profiler.h"
#include "blt/std/logging.h"
#include "blt/std/time.h"

int main() {
    binaryTreeTest();
    
    BLT_TRACE("Hello World!\n");
    BLT_DEBUG("Hello World!\n");
    BLT_INFO("Hello World!\n");
    BLT_WARN("Hello World!\n");
    BLT_ERROR("Hello World!\n");
    BLT_FATAL("Hello World!\n");
    
    BLT_TRACE_LN("Hello World!\n");
    BLT_DEBUG_LN("Hello World!\n");
    BLT_INFO_LN("Hello World!\n");
    BLT_WARN_LN("Hello World!");
    BLT_ERROR_LN("Hello World!");
    BLT_FATAL_LN("Hello World!");
    
    std::string hello = "superSexyMax";
    std::cout << "String starts with: " << blt::string::contains(hello, "superSexyMaxE") << "\n";
    
    return 0;
}