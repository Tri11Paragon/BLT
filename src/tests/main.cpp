
#include <unordered_map>
#include "binary_trees.h"
#include "logging.h"
#include "profiling_tests.h"
#include "nbt_tests.h"
#include "queue_tests.h"
#include <bitset>

int main() {
    
    int data = 0;
    int index = 2;
    data = data | index << (32 - 2);
    auto fdata = *reinterpret_cast<float*>(&data);
    
    auto back_to_int = *reinterpret_cast<int*>(&fdata);
    
    std::string bits = std::bitset<32>((unsigned long long) back_to_int).to_string();
    std::cout << bits << "\n";
    return 0;
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