
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

int main(int argc, char** argv) {
    
    if (argc > 1 && std::string(argv[1]) == "--no_color") {
        for (int i = (int)blt::logging::log_level::NONE; i < (int)blt::logging::log_level::FATAL; i++) {
            blt::logging::setLogColor((blt::logging::log_level)i, "");
        }
        blt::logging::setLogOutputFormat("[${{TIME}}] [${{LOG_LEVEL}}] (${{FILE}}:${{LINE}}) ${{STR}}\n");
    }
    
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
    
    constexpr int size = 100;
    uint32_t vals[size];
    
    for (uint32_t & val : vals)
        val = 0;
    
    uint32_t seed = 1023;
    for (int i = 0; i < 10000000; i++){
        seed = seed * i;
        vals[blt::random::randomInt(seed, 0, size)] += 1;
    }
    
    uint32_t mean = 0;
    for (uint32_t& val : vals)
        mean += val;
    mean /= size;
    
    uint32_t std = 0;
    for (uint32_t& val : vals) {
        auto e = (val - mean);
        std += e * e;
    }
    
    auto stdev = sqrt((double)std / (double)size);
    
    BLT_INFO("STDDEV of # random values: %f", stdev);
    
    return 0;
}