
#include <unordered_map>
#include "binary_trees.h"
#include "blt/std/string.h"
#include "blt/profiling/profiler.h"
#include "blt/std/logging.h"
#include "blt/std/time.h"
#include "blt/std/format.h"
#include <chrono>
#include <thread>

void print(const std::vector<std::string>& vtr) {
    for (const auto& line : vtr)
        BLT_TRACE(line);
}

int main() {
    binaryTreeTest();
    
    BLT_START_INTERVAL("Help", "SuperSet");
    
    BLT_TRACE(10);
    BLT_TRACE(10.5);
    
    BLT_TRACE("Hello %d World!\n", 50);
    BLT_DEBUG("Hello %E World!\n", 1205200.0);
    BLT_INFO("Hello World!\n");
    BLT_WARN("Hello World!\n");
    BLT_ERROR("Hello World!\n");
    BLT_FATAL("Hello World!\n");
    
    blt::logging::tlog << "Hello! " << "Double stream insertion! " << 51 << 65 << " ";
    blt::logging::tlog << "Same Line! ";
    blt::logging::tlog << "Write the end!\n";
    blt::logging::tlog << "Seeee\n Super\n";
    
    std::string hello = "superSexyMax";
    std::cout << "String starts with: " << blt::string::contains(hello, "superSexyMaxE") << "\n";
    
    BLT_END_INTERVAL("Help", "SuperSet");
    
    BLT_START_INTERVAL("Help", "SecondSet");
    for (int i = 0; i < 1000; i++){
        ;;
    }
    BLT_END_INTERVAL("Help", "SecondSet");
    BLT_START_INTERVAL("Help", "UnderSet");
    for (int i = 0; i < 1000; i++){
        ;;
    }
    BLT_END_INTERVAL("Help", "UnderSet");
    
    for (int i = 0; i < 15; i++) {
        BLT_START_INTERVAL("Help", "UnderSet" + std::to_string(i));
        BLT_END_INTERVAL("Help", "UnderSet" + std::to_string(i));
    }
    
    BLT_PRINT_PROFILE("Help", blt::logging::LOG_LEVEL::TRACE);
    BLT_PRINT_ORDERED("Help", blt::logging::LOG_LEVEL::TRACE);
    
    blt::string::TableFormatter formatter;
    formatter.addColumn(blt::string::TableColumn{"Test1"});
    formatter.addColumn(blt::string::TableColumn{"Test2"});
    formatter.addColumn(blt::string::TableColumn{"Test Column"});
    formatter.addColumn(blt::string::TableColumn{"Test3"});
    
    formatter.addRow({"This", "This LARGE", "222", "5000"});
    formatter.addRow({"OTHER", "LARGE", "6", "1"});
    formatter.addRow({"hheee", "looo", "m8", "loser"});
    
    print(formatter.createTable(true, true));
    BLT_TRACE("");
    print(formatter.createTable(false, true));
    BLT_TRACE("");
    print(formatter.createTable(true, false));
    BLT_TRACE("");
    print(formatter.createTable(false, false));
    
    return 0;
}