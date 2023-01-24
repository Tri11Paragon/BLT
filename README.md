# **BLT**

![Icon](icon_large.png)

### A common utilties library of missing stl features. (and more!)

---

# **Features**
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