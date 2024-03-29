# **BLT v0.8.0a**
A C++20 common utilities library to make thing easy! 

![Icon](icon_large.png)

---

# Specialties
## blt::logging.v2 
Found in the header file `include/std/logging.h`, blt::logging is a fast and simple
single header logging library which can be used independently of BLT. Just include
the header file anywhere in your project and in ONE cpp file define `#define BLT_LOGGING_IMPLEMENTATION`.
If you are using BLT as a CMake library (as you should!) this is done for you.
- ### Compile Time Config
  - Using CMake
    - Several options are provided which disable various logging contexts, as such global logging can be disabled by passing `-DBLT_DISABLE_LOGGING:BOOL=ON`
    - Options follow the pattern of `BLT_DISABLE_LEVEL` where level is one of `TRACE`,`DEBUG`,`INFO`,`WARN`,`ERROR`, or `FATAL`.
      - This allows for individual logging levels to be disabled while leaving the others functional. These options can be combined.
      - See CMakeLists.txt for a complete list of options.
  - Standalone
    - The CMake options define global variables in a config file. If you are using logging standalone you will need to remove the config include.
    - Macros can be used in the same pattern of `BLT_DISABLE_LEVEL` to disable the various logging levels
## blt::arg_parse
blt::arg_parse found in `blt/parse/argparse.h` is an attempt at a near 1:1 replication of Python's argparse in C++. 
Python's argparse is one of the best command line argument parsing solutions with some of the best documentation I've seen. 
The goal was to create an API which would function exactly as the Python docs described, simplifying how much I needed to remember and document for this library.
## blt::profile_t (v2)
The newest version of my profiler solution now features a CPU cycle counter, CPU thread execution time, and wall time. It has more options for
sorting and general printing while featuring a nicer codebase. It is an improvement over blt::profiling while maintaining (almost) complete backwards
compatability. Due to changes in the ABI the `BLT_PRINT_PROFILE` macro deletes the internal profiles and intervals AND takes arguments which match
the new formatting options for profiling v2. However, `BLT_START_INTERVAL`, `BLT_WRITE_PROFILE`, and `BLT_END_INTERVAL` are still the same.
Documentation for this is coming soon, along with more profiling features. the `BLT_*` macros can be disabled by the standard `BLT_DISABLE_PROFILING`
from the v1 profiler. It is encouraged to use the new blt::* profile functions over the macros however these currently cannot be disabled. (TODO)
---

# **Features / Examples**
- ## Data Structures
  - Queue / Stack 
    - faster than std::queue / std::stack
    - backed by a contiguous array
- ## Utility
  - Simple Random Wrapper Interface
  - Simple random functions based on the PCG Hash
  - ### String Functions
    - starts_with
    - ends_with
    - contains
    - toLowerCase
    - toUpperCase
    - split
    - trim
  - Logging
    - See blt::logging section above
  - Time
    - Current time in nanoseconds (without all the c++ gobbledygook)
      - Java's currentTimeMilliseconds
      - nanoTime as well
      - `std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()` becomes `blt::system::nanoTime()`
    - Formatted time string with year/month/date + current time
- ## Profiling
  - Basic profiler with history and formatted output