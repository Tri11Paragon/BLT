
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
    
    blt::logging::flush();
    
    nbt_tests();
    
    BLT_TRACE0_STREAM << "Test Output!\n";
    BLT_TRACE1_STREAM << 5 << "\n";
    BLT_TRACE2_STREAM << 5 << "\n";
    BLT_TRACE3_STREAM << 5 << "\n";
    BLT_TRACE_STREAM << "TRACEY\n";
    
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
    
    BLT_TRACE("Hello Trace!");
    BLT_DEBUG("Hello Debug!");
    BLT_INFO("Hello Info!");
    BLT_WARN("Hello Warn!");
    BLT_ERROR("Hello Error!");
    BLT_FATAL("Hello Fatal!");
    
    int vals[100];
    
    for (int & val : vals)
        val = 0;
    
    uint32_t seed = 1023;
    for (int i = 0; i < 1203000; i++){
        seed = seed * i;
        vals[blt::random::randomInt(seed, 0, 100)]++;
    }
    
    std::fstream csv("./randoms2.csv", std::ios::out);
    
    csv << "index,count\n";
    for (int i = 0; i < 100; i++){
        csv << i << "," << vals[i] << "\n";
    }
    
    csv.close();
    
    return 0;
}