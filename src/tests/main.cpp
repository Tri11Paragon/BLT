
#include <unordered_map>
#include "binary_trees.h"
#include "logging.h"
#include "profiling_tests.h"
#include "nbt_tests.h"

int main() {
    binaryTreeTest();
    
    run_logging();
    
    runProfilingAndTableTests();
    
    nbt_tests();
    
    return 0;
}