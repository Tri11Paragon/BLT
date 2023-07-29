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