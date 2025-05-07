# Enchantum

**Enchantum** (short for "enchant enum") is a modern **C++20** header-only library for **compile-time enum reflection**. It provides fast, lightweight access to enum values, names, and bitflags — all built with clean, idiomatic C++ in mind.

> Every year, countless turtles perish due to the pollution caused by slow, bloated build times.  
 Save the turtles — and your compile times — by switching to **Enchantum**.

**Source:** I made it up.


[Why another enum reflection library?](#why-another-enum-reflection-library)

[Features](docs/features.md)

[Limitations](docs/limitations.md)

[CMake Integration](#cmake-integration)

Tested locally with:
- Visual Studio 2022 (v17.13.6)
- GCC 14.2.0
- Clang 20.1.2

---

## Why Another Enum Reflection Library?

There are several enum reflection libraries out there — so why use **Enchantum** instead of [magic_enum](https://github.com/Neargye/magic_enum), [simple_enum](https://github.com/arturbac/simple_enum), or [conjure_enum](https://github.com/fix8mt/conjure_enum)?

### magic_enum

**Pros**
- Header-only and macro-free
- Supports C++17

**Cons**
- Compile times grow significantly with larger `MAGIC_ENUM_MAX_RANGE`
- Requires alternate APIs like `magic_enum::enum_name<E::V>()` to mitigate compile-time costs

### conjure_enum

*Note: Could not get this to compile locally. Based on the README, compile times are similar to or worse than magic_enum.*

### simple_enum

**Pros**
- Fast compile times but only if range is small

**Cons**
- Requires specifying enum first/last values manually
- Compile time slows down with large enum ranges

### enchantum

**Pros**
- Avoids intrusive macros
- Does not sacrifice API ease of use and features for compile time sake (e.g no `ENCHANTUM_ALL_ENUMS_ARE_CONTIGUOUS_OPTIMIZATION` flag which would disable support for sparse enums)
- No manual range definitions required for each enum  (but supported if desired)
- Delivers fast compile times
- Clean and Simple API `enchantum::to_string(E)` no `enchantum::to_string<E::V>()` since compile times are fast
- More features like disabling null termination if not needed
- Supports all sort of enums (scoped,unscoped,C style unfixed underlying type,anonymous namespaced enums, enums with commas in their typename,etc...);

**Cons**
- C++20 required

---

## Compile-Time Benchmarks

Each benchmark was run 10 times (except MSVC which was ran 3 times) and averaged unless noted otherwise.
`range` is `ENCHANTUM_MAX_RANGE` and `MAGIC_ENUM_RANGE_MAX`
### Small Enums  
*200 enums, 16 values each, range: -128 to 128*

| Compiler    | `magic_enum` (s) | `enchantum` (s) | Time Saved |
| ----------- | ---------------- | --------------- | ---------- |
| MSVC        | 80               | 22              | 58         |
| GCC         | 47               | 7               | 40         |
| Clang       | 47               | 8               | 39         |

### Large Enums  
*32 enums, 200 values each, range: -256 to 256*

| Compiler    | `magic_enum` (s) | `Enchantum` (s) | Time Saved |
| ----------- | ---------------- | --------------- | ---------- |
| MSVC        | 37               | 15              | 22         |
| Clang       | 18               | 5               | 13         |
| GCC         | 21               | 6               | 15         |


### Very Large Enum Range  
*200 enums, 16 values each, range: -1024 to 1024*

*Only ran twice due to extreme compilation times.*

| Compiler | magic_enum       | Enchantum     |
|----------|------------------|---------------|
| MSVC     | >20 min (killed) | ~107 sec      |
| GCC      | >15 min (killed) | ~37 sec       |
| Clang    | >15 min (killed) | ~42 sec       |
---

## Summary

**Enchantum** significantly reduces compile times in enum-heavy projects. In my own project, switching from magic_enum reduced full rebuild times from about 2 minutes to 1 minute and 30 seconds which I was very surprised that `magic_enum` alone took 30 seconds.

The only trade-off is that Enchantum requires C++20, while `magic_enum` supports C++17.

---

## Examples
### Enum to String
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

### Iterating over values
```cpp
#include <enchantum/enchantum.hpp>

enum class Status { Ok = 0, Warning = 1, Error = 2 };

for (auto& [value,string]: enchantum::entries<Status>) {
    std::cout << string  << " = " static_cast<int>(value) << '\n';
}
// Outputs:
// Ok = 0
// Warning = 1
// Error = 2
```

### Checking for validity
```cpp
#include <enchantum/enchantum.hpp>

enum class Direction { North, South, East, West };

std::cout << std::boolalpha << enchantum::contains<Direction>("North") << '\n'; // true
std::cout << std::boolalpha << enchantum::contains<Direction>("Up") << '\n'; // false
std::cout << std::boolalpha << enchantum::contains(Direction(-42)) << '\n'; // false
std::cout << std::boolalpha << enchantum::contains<Direction>(0) << '\n'; // true
```
### Min Max Count
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