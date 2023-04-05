
#include <unordered_map>
#include "binary_trees.h"
#include "logging.h"
#include "profiling_tests.h"
#include "nbt_tests.h"
#include "queue_tests.h"
#include "blt/math/vectors.h"
#include "blt/math/matrix.h"
#include <bitset>
#include "hashmap_tests.h"

int main() {
    binaryTreeTest();
    
    run_logging();
    
    runProfilingAndTableTests();
    
    nbt_tests();
    
    blt::logging::trace << "Test Output!\n";
    blt::logging::trace << 5;
    blt::logging::trace << 5;
    blt::logging::trace << 5;
    
    blt::logging::flush();
    
    test_queues();
    
    blt::vec4 v{2, 5, 1, 8};
    blt::mat4x4 m{};
    m.m(0,0, 1);
    m.m(0,2, 2);
    m.m(1, 1, 3);
    m.m(1, 3, 4);
    m.m(2, 2, 5);
    m.m(3, 0, 6);
    m.m(3, 3, 7);
    
    auto result = m * v;
    
    std::cout << result.x() << " " << result.y() << " " << result.z() << " " << result.w() << std::endl;
    
    if (test_hashmaps()){
        BLT_FATAL("Hashmap test failed!");
        return 1;
    }
    
    return 0;
}