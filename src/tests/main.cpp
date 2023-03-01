
#include <unordered_map>
#include "binary_trees.h"
#include "logging.h"
#include "profiling_tests.h"
#include "nbt_tests.h"
#include "queue_tests.h"

int main() {
    binaryTreeTest();
    
    run_logging();
    
    runProfilingAndTableTests();
    
    nbt_tests();
    
    blt::logging::tlog << "Test Output!\n";
    blt::logging::tlog << 5;
    blt::logging::flog << 5;
    blt::logging::ilog << 5;
    
    blt::logging::flush();
    
    test_queues();
    
    return 0;
}