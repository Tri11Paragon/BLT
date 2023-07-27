
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
#include <functional>

std::function<int(int i)> test{
    [](int i) -> int {
        int acc = 1;
        for (int j = 0; j < i; j++){
            acc += j * i;
        }
        return acc;
    }
};

int test_as_func(int i){
    int acc = 1;
    for (int j = 0; j < i; j++){
        acc += j * i;
    }
    return acc;
}

inline int test_as_func_inline(int i){
    int acc = 1;
    for (int j = 0; j < i; j++){
        acc += j * i;
    }
    return acc;
}

std::function<int(int i)> test_func_as_std(&test_as_func);

class super_func {
    public:
        virtual int test(int i) = 0;
        virtual ~super_func() = default;
};

class class_func : public super_func {
    public:
        int test(int i) override {
            int acc = 1;
            for (int j = 0; j < i; j++){
                acc += j * i;
            }
            return acc;
        }
};

int (*func_func)(int) = [](int i) -> int {
    int acc = 1;
    for (int j = 0; j < i; j++){
        acc += j * i;
    }
    return acc;
};

int (*func_func_in)(int) = &test_as_func;

int main(int argc, char** argv) {
    
    if (argc > 1 && std::string(argv[1]) == "--no_color") {
        for (int i = (int)blt::logging::log_level::NONE; i < (int)blt::logging::log_level::FATAL; i++) {
            blt::logging::setLogColor((blt::logging::log_level)i, "");
        }
        blt::logging::setLogOutputFormat("[${{TIME}}] [${{LOG_LEVEL}}] (${{FILE}}:${{LINE}}) ${{STR}}\n");
    }
    
    auto* funy = new class_func;
    super_func* virtual_funy = new class_func;
    
    for (int _ = 0; _ < 10; _++ ) {
        int num_of_tests = 10000;
        int acc = 1;
        BLT_START_INTERVAL("Functions Test", "std::function (lambda)");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += test(i);
        }
        BLT_END_INTERVAL("Functions Test", "std::function (lambda)");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "std::function (normal)");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += test_func_as_std(i);
        }
        BLT_END_INTERVAL("Functions Test", "std::function (normal)");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "normal function");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += test_as_func(i);
        }
        BLT_END_INTERVAL("Functions Test", "normal function");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "(inline) normal function");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += test_as_func_inline(i);
        }
        BLT_END_INTERVAL("Functions Test", "(inline) normal function");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "virtual class direct");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += funy->test(i);
        }
        BLT_END_INTERVAL("Functions Test", "virtual class direct");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "virtual class");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += virtual_funy->test(i);
        }
        BLT_END_INTERVAL("Functions Test", "virtual class");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "funcptr lambda");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += func_func(i);
        }
        BLT_END_INTERVAL("Functions Test", "funcptr lambda");
        BLT_TRACE(acc);
        
        BLT_START_INTERVAL("Functions Test", "c function ptr");
        acc = 1;
        for (int i = 0; i < num_of_tests; i++) {
            acc += func_func_in(i);
        }
        BLT_END_INTERVAL("Functions Test", "c function ptr");
        BLT_TRACE(acc);
    }
    
    BLT_PRINT_PROFILE("Functions Test", blt::logging::log_level::NONE, true);
    delete virtual_funy;
    
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
    
    std::fstream nbtFile("super_file.nbt", std::ios::out | std::ios::binary);
    blt::fs::fstream_block_writer blockWriter(nbtFile);
    blt::nbt::NBTWriter nbtWriter(blockWriter);
    nbtWriter.write(new blt::nbt::tag_compound("root", {
        new blt::nbt::tag_byte("super_byte", 8),
        new blt::nbt::tag_short("shortTest", 32767),
        new blt::nbt::tag_compound("SEXY_COMPOUND", {
            new blt::nbt::tag_list("my list", {
                new blt::nbt::tag_long("", 1230),
                new blt::nbt::tag_long("", 2),
                new blt::nbt::tag_long("", 50340535),
                new blt::nbt::tag_long("", 55),
                new blt::nbt::tag_long("", 256),
                new blt::nbt::tag_long("", 512),
                new blt::nbt::tag_long("", 9999999999),
            }),
            new blt::nbt::tag_double("OMG IT'S A DOUBLE", 1320.04324),
            new blt::nbt::tag_float("OMG IT'S A FLOAT", 12.04324),
            new blt::nbt::tag_compound("Triple", {
                new blt::nbt::tag_int("Test int", 32),
                new blt::nbt::tag_byte_array("super array", {
                        51, 23, 12, 04, 33, 53, 11, 22, 3, 93, 120
                }),
                new blt::nbt::tag_string("I am a string", "I have stringy contents"),
                new blt::nbt::tag_string("name", "Bananrama"),
                new blt::nbt::tag_int_array("int array", {
                        1230, 234023, 21300, 2309230, 2340230, 2, 1, 32, 3265, 12, 53, 123, 7, 56, 12
                }),
                new blt::nbt::tag_long_array("valid", {
                        1230, 5320, 323200234402304, 230023, 23042034, 230230, 2301203, 123010230, 12300123
                })
            })
        })
    }));
    
    return 0;
}