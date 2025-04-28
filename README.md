# Enchantum

**Enchantum** (enchant enum) is a modern **C++20** library for **compile-time enum reflection**. It offers fast and lightweight handling of enum values, names, and bitflags — all built with clean C++ concepts in mind.

Note: Currently supported on **MSVC** and **GCC**. Clang support is very coming soon TM.

Tested with **Visual Studio 2022 (v17.13.6)** and **GCC (14.2.0)**.

>Every year, countless turtles perish due to the pollution created by large build servers. Save the turtles—and your build times—by switching to **Enchantum** for faster enum reflection! 

**Source**: I made it up.

[Features](docs/features.md)

[CMake Integration](#cmake-integration)


# Why yet another enum reflection library?

Why would you want to use this library over [`magic_enum`](https://github.com/Neargye/magic_enum) or [`simple_enum`](https://github.com/arturbac/simple_enum) or [`conjure_enum`](https://github.com/fix8mt/conjure_enum)?

[`magic_enum`](https://github.com/Neargye/magic_enum) 

**advantages**
   - macro free (no annoying intrusive macros)
   - C++17 support

**disadvantages**
   - slow in compile times and it increases exponentially the higher `MAGIC_ENUM_MAX_RANGE` macro is


[`conjure_enum`](https://github.com/fix8mt/conjure_enum)

didn't personally test it since I could not get it to compile but from the readme of it it says it is like `magic_enum` in compile times or even worse.


[`simple_enum`](https://github.com/arturbac/simple_enum)

**advantages**
   - fast compiling

**disadvantages**
   - requires modification to existing code (specifying last/first)
   - slow compiling if the `last/first` range is too big


If you're looking for faster compile times with enum reflection-heavy code without annoying manual specifying of ranges or macros, **Enchantum** delivers while still being fast in compile times.



## Compile Time Benchmarks

All tests were performed with `enchantum::to_string(x)` / `magic_enum::enum_name(x)` calls over various enum sizes.

Each test was run 3 times and averaged unless otherwise noted.

### Small Enums (200 enums, 16 values each, range: -128 to 128)

| Compiler | `magic_enum` (secs) | `enchantum` (secs) | Saved Time |
|----------|-------------------|------------------|----------------|
| MSVC     | 80.63            | 22.06            | 72.64%         |
| GCC      | 39.01             | 8.91             | 77.16%         |


### Large Enums (32 enums, 200 values each, range: -256 to 256)

| Compiler | `magic_enum` (secs) | `enchantum` (secs) | Saved Time |
|----------|----------------------|---------------------|----------------|
| MSVC     | 37.03               | 14.17               | 61.72%         |
| GCC      |  18.40                | 6.78                | 63.15%         |

---

### Very Large Enum Range (200 enums, 16 values each, range: -1024 to 1024)

*Only ran once due to long compilation times.*

| Compiler | magic_enum          | enchantum |
|----------|---------------------|-----------|
| MSVC     | >20 mins (killed I got bored)   | ~120 secs   |
| GCC      | >15 mins (killed I got bored)   | ~70 secs   |


---

## Summary

**Enchantum** massively reduces compile times in enum-heavy projects compared to [`magic_enum`](https://github.com/Neargye/magic_enum), especially at larger scales and ranges like  `-1024` to `1024` test above it is a difference of 1 minute to not even finishing compilation and making my laptop a heater.

Also personally trying my library on my project saved the compile times from ~2 minutes for a full rebuild to just ~1 minute and 30 seconds I was very surprised magic enum took whole 30 seconds.

Well an advantage to [`magic_enum`](https://github.com/Neargye/magic_enum) is its C++17 support.

---




# Examples
_Look at tests for more examples_
## Basic Enum to String

```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/ostream.hpp>
using namespace enchantum::ostream_operators;

enum class Color { Red, Green, Blue };

Color c = Color::Green;
std::string_view name = enchantum::to_string(c);
std::cout << name << '\n'; // Outputs: Green

std::cout << c << '\n'; // Outputs: Green
```

## Iterating over values
```cpp
#include <enchantum/enchantum.hpp>

enum class Status { Ok = 0, Warning = 1, Error = 2 };

for (auto [value,string]: enchantum::entries<Status>) {
    std::cout << string  << " = " static_cast<int>(value) << '\n';
}
// Outputs:
// Ok = 0
// Warning = 1
// Error = 2
```

## Checking for validity
```cpp
#include <enchantum/enchantum.hpp>

enum class Direction { North, South, East, West };

std::cout << std::boolalpha << enchantum::contains<Direction>("North") << '\n'; // true
std::cout << std::boolalpha << enchantum::contains<Direction>("Up") << '\n'; // false
std::cout << std::boolalpha << enchantum::contains(Direction(-42)) << '\n'; // false
std::cout << std::boolalpha << enchantum::contains<Direction>(0) << '\n'; // true
```
## Min Max Count
```cpp

enum class Errno { BadSomething = -1, IamGood = 0, IAmBadV2 = 1 };

std::cout << "Min: " << static_cast<int>(enchantum::min<Errno>) << '\n'; // -1 BadSomething
std::cout << "Max: " << static_cast<int>(enchantum::max<Errno>) << '\n'; // 1 IAmBadV2
std::cout << "Count: " << enchantum::count<Errno> << '\n'; // 3
```

---


# CMake Integration

The **cmake** file provides the target `enchantum::enchantum` since this library is header-only it is very simple to use you can copy paste the files and add the include directory or use **cmake** and the library as a submodule. 

```cpp
add_subdirectory("third_party/enchantum")
target_link_libraries(your_executable enchantum::enchantum)
```