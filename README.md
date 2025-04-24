# Enchantum

**Enchantum** is a modern **C++20** library for **compile-time enum reflection**. It offers fast and lightweight handling of enum values, names, and bitflags — all built with clean C++ concepts in mind.

Note: Currently supported on **MSVC** and **GCC**. Clang support is coming soon TM.

Note 2: **GCC** implementation currently does not support C style enums.  

Tested with **Visual Studio 2022 (v17.13.6)**.

[Features](docs/features.md)

# Examples

## Basic Enum to String

```cpp
#include <enchantum/enchantum.hpp>

enum class Color { Red, Green, Blue };

Color c = Color::Green;
std::string_view name = enchantum::to_string(c);
std::cout << name << '\n'; // Outputs: Green
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

| Compiler | Library      | Time           | Notes                                |
|----------|--------------|----------------|--------------------------------------|
| MSVC     | magic_enum   | 1:20.626 mins  |                                      |
| MSVC     | enchantum    | 33.416   secs  | `ENCHANTUM_SEARCH_ITER_COUNT = 32`   |
| GCC      | magic_enum   | 39.01    secs  |                                      |
| GCC      | enchantum    | 18.40    secs  | `ENCHANTUM_SEARCH_ITER_COUNT = 32`   |
| Clang    | magic_enum   | Not supported  |                                      |
| Clang    | enchantum    | Not supported  |                                      |

---

### Large Enums (32 enums, 200 values each, range: -256 to 256)

| Compiler | Library      | Time           | Notes                                |
|----------|--------------|----------------|--------------------------------------|
| MSVC     | magic_enum   | 37.032 secs    |                                      |
| MSVC     | enchantum    | 22.212 secs    | `ENCHANTUM_SEARCH_ITER_COUNT = 32`   |
| GCC      | magic_enum   | 18.40    secs  |                                      |
| GCC      | enchantum    | 10.60    secs  | `ENCHANTUM_SEARCH_ITER_COUNT = 32`   |
| Clang    | magic_enum   | Not supported  |                                      |
| Clang    | enchantum    | Not supported  |                                      |

---

### Very Large Enum Range (200 enums, 16 values each, range: -1024 to 1024)

*Only ran once due to long compilation times.*

| Compiler | Library      | Time           | Notes                                          |
|----------|--------------|----------------|------------------------------------------------|
| MSVC     | magic_enum   | >20 mins (killed I got bored) | Compiler did not finish         |
| MSVC     | enchantum    | 3:12.947 mins   | `ENCHANTUM_SEARCH_ITER_COUNT = 32`            |
| GCC      | magic_enum   | >15 mins (killed I got bored as well)  | Compiler did not finish|
| GCC      | enchantum    | 6:21.57 mins    | `ENCHANTUM_SEARCH_ITER_COUNT = 32`            |
| Clang    | magic_enum   | Not supported   |                                               |
| Clang    | enchantum    | Not supported   |                                               |

---

## Summary

**Enchantum** significantly reduces compile times in enum-heavy projects compared to [`magic_enum`](https://github.com/Neargye/magic_enum), especially at larger scales.

---
