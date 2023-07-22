# **BLT v0.6.0a**
A common utilities library I find useful

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
    - 
---

# **Features / Examples**
- ## Data Structures
  - Queue / Stack 
    - faster than std::queue / std::stack
  - Binary Tree
  - Hashmap (TODO)
- ## Utility
  - Simple Random Interface
    - No more worrying about min/max bounds!
  - ### String Functions
    - starts_with
    - ends_with
    - contains
    - toLowerCase
    - toUpperCase
    - split
    - trim
  - Logging
    - Trace / Debug / Info / Warn / Error / Fatal
    - Log to file
    - Log to console with color!
    - Easy to disable for release
      - define BLT_DISABLE_LOGGING before including the logging.h file.
  - Time
    - Current time in nanoseconds (without all the c++ gobbledygook)
    - Formatted time string with year/month/date + current time
- ## Profiling
  - Basic profiler
    - WIP