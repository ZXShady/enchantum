# Enchantum

**Enchantum** (enchant enum) is a modern **C++20** library for **compile-time enum reflection**. It offers fast and lightweight handling of enum values, names, and bitflags — all built with clean C++ concepts in mind.

Note: Currently supported on **MSVC** and **GCC**. Clang support is very coming soon TM.

Tested with **Visual Studio 2022 (v17.13.6)** and **GCC (14.2.0)**.

>Every year, countless turtles perish due to the pollution created by large build servers. Save the turtles—and your build times—by switching to **Enchantum** for faster enum reflection! 

**Source**: I made it up.

[Features](docs/features.md)

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

std::cout << "Min: " << static_cast<int>(enchantum::min<Result>) << '\n'; // -1 BadSomething
std::cout << "Max: " << static_cast<int>(enchantum::max<Result>) << '\n'; // 1 IAmBadV2
std::cout << "Count: " << enchantum::count<Result> << '\n'; // 3
```

---

## Why use Enchantum over [`magic_enum`](https://github.com/Neargye/magic_enum)?

If you're looking for faster compile times with enum reflection-heavy code, **Enchantum** delivers.

## Compile Time Benchmarks (per Compiler)

All tests were performed with `enchantum::to_string` / `magic_enum::enum_name` calls over various enum sizes.

Each test was run 3 times and averaged unless otherwise noted.

### Small Enums (200 enums, 16 values each, range: -128 to 128)

| Compiler | Library      | Time           |
|----------|--------------|----------------|
| MSVC     | magic_enum   | 1:20.626 mins  |
| MSVC     | enchantum    | 22.06    secs  |
| GCC      | magic_enum   | 39.01    secs  |
| GCC      | enchantum    | 8.91     secs  |
| Clang    | magic_enum   | Not supported  |
| Clang    | enchantum    | Not supported  |

---

### Large Enums (32 enums, 200 values each, range: -256 to 256)

| Compiler | Library      | Time           |
|----------|--------------|----------------|
| MSVC     | magic_enum   | 37.032   secs  |
| MSVC     | enchantum    | 14.17    secs  |
| GCC      | magic_enum   | 18.40    secs  |
| GCC      | enchantum    | 6.78     secs  |
| Clang    | magic_enum   | Not supported  |
| Clang    | enchantum    | Not supported  |

---

### Very Large Enum Range (200 enums, 16 values each, range: -1024 to 1024)

*Only ran once due to long compilation times.*

| Compiler | Library      | Time                                   |
|----------|--------------|----------------------------------------|
| MSVC     | magic_enum   | >20 mins (killed I got bored)          |
| MSVC     | enchantum    | ~2 mins                                |
| GCC      | magic_enum   | >15 mins (killed I got bored as well)  |
| GCC      | enchantum    | 1 min 10 secs                          |
| Clang    | magic_enum   | Not supported                          |
| Clang    | enchantum    | Not supported                          |

---

## Summary

**Enchantum** massively reduces compile times in enum-heavy projects compared to [`magic_enum`](https://github.com/Neargye/magic_enum), especially at larger scales and ranges like  `-1024` to `1024` test above it is a difference of 1 minute to not even finishing compilation and making my laptop a heater.

Also personally trying my library on my project saved the compile times from ~2 minutes for a full rebuild to just ~1 minute and 30 seconds I was very surprised magic enum took whole 30 seconds.

Well an advantage to [`magic_enum`](https://github.com/Neargye/magic_enum) is its C++17 support,
I guess it is an advantage to [`conjure_enum`](https://github.com/fix8mt/conjure_enum)

---
