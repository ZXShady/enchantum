# Enchantum

**Enchantum** (short for "enchant enum") is a modern **C++20** header-only library for **compile-time enum reflection**. It provides fast, lightweight access to enum values, names, and bitflags.

> Every year, countless turtles perish due to the pollution caused by slow, bloated build times.  
 Save the turtles — and your compile times — by switching to enchantum!!!.

**Source:** I made it up.


[Simple Example](#simple-example)

[Why another enum reflection library?](#why-another-enum-reflection-library)

[Features](docs/features.md)

[Limitations](docs/limitations.md)

[CMake Integration](#cmake-integration)

Tested locally on Windows 10 with:
- Visual Studio 2022 (v17.13.6)
- GCC 14.2.0
- Clang 20.1.2

---

## Simple Example

```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/iostream.hpp>


enum class Music { Rock = 42, Jazz = 21, Metal };

enum class Enemy { 
    None = 0,
    Fast = 1 << 0,
    Melee = 1 << 1,
    Ranged = 1 << 2
};
ENCHANTUM_DEFINE_BITWISE_FOR(Enemy) // Defines bitwise operators (~, &, |, ^, &=, |=, ^=) for the Enemy enum. See docs/features.md#enchantum_define_bitwise_for for details.

int main() {
  auto music = Music::Rock;
  std::string_view music_name =  enchantum::to_string(music);
  // music_name == "Rock"
  
  auto caster = enchantum::cast<Music>; // most "functions" are functors!

  std::optional<Music> music_jazz = caster("Jazz");
  if (music_jazz.has_value()) {
    // *music_jazz == Music::Jazz
  }



  std::optional<std::size_t> jazzy_index = enchantum::enum_to_index(Music::Jazz);
  if(jazzy_index.has_value()){
    std::optional<Music> jazz_from_index = enchantum::index_to_enum<Music>(*jazzy_index);
    // *jazz_from_index == Music::Jazz
  }

  static_assert(enchantum::count<Music> == 3);
  for(Music types : enchantum::values<Music>) { // variable based API
    using namespace enchantum::iostream_operators; // Provides std::ostream operator<< and std::istream operator>> for enums. See docs/features.md#stream-operators.
    std::cout << types << '\n'; // calls enchantum::to_string
  }
  for(std::string_view names : enchantum::names<Music>) // variable based API
    std::cout << names << '\n';
  
  // Both Output:
  // Rock
  // Jazz
  // Metal


  for(auto [value,name] : enchantum::entries<Music>) // variable based API
    std::cout << enchantum::to_underlying(value) << " = " << name << '\n';
 
 
   std::string enemy_string = enchantum::to_string_bitflag(Enemy::Fast | Enemy::Melee); 
   // enemy "Fast|Melee"
   std::optional<Enemy> enemy_from_string = enchantum::cast_bitflag<Enemy>(enemy_string);
   // *enemy_from_string == Enemy::Fast | Enemy::Melee
}


```

## Why Another Enum Reflection Library?

There are several enum reflection libraries out there — so why choose **enchantum** instead of [magic_enum](https://github.com/Neargye/magic_enum), [simple_enum](https://github.com/arturbac/simple_enum), or [conjure_enum](https://github.com/fix8mt/conjure_enum)?

### magic_enum

**Pros**
- Macro-free (non intrusive)
- No modifications needed for existing enums
- Allows specifying ranges for specific enums when needed
- Supports C++17
- Nicer compiler errors
- Supports wide strings.

**Cons**
- Compile times grow significantly with larger `MAGIC_ENUM_MAX_RANGE`. 
- Requires alternate APIs like `magic_enum::enum_name<E::V>()` to mitigate said compile-time costs. which forces those functions not to be functors.
### conjure_enum

**Pros**
- Macro-free (non intrusive)
- Uses C++20

**Cons**

*Note: Could not get this to compile locally. Based on the README, compile times are similar to or worse than magic_enum.*

### simple_enum

**Pros**
- Fast compile times but only if range is small
- Functor based api.

**Cons**
- Requires specifying enum first/last values manually (instrusive does not mix well with third party enums)
- Compile time slows down with large enum ranges

### enchantum

**Pros**
- Macro-free (non intrusive)
- Aims to provide a balance of features and API usability without overly compromising compile-time performance (e.g., supports sparse enums without requiring optimization flags that disable such support).
- Allows specifying ranges for specific enums when needed via `enum_traits`.
- A design goal is efficient compile times (see Benchmarks section for observed performance).
- Offers a functor-based API (e.g., `enchantum::to_string(E)`).
- Provides features such as optional null termination for string views and reflection of '0' values for bitflag enums.
- Supports a wide variety of enum types (scoped, unscoped, C-style with unfixed underlying types, anonymous namespaced enums, etc.).

**Cons**
- C++20 required
- Compiler errors are incomprehensible if something goes wrong, needs a level 10 wizard to read them (though see `docs/limitations.md#other-compiler-issues` for common issues and solutions).
- No support for wide strings (yet)
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

*Only ran 2 times due to extreme compilation times.*

| Compiler | magic_enum       | Enchantum     |
|----------|------------------|---------------|
| MSVC     | >20 min (killed) | ~107 sec      |
| GCC      | >15 min (killed) | ~37 sec       |
| Clang    | >15 min (killed) | ~42 sec       |
---

## Key Performance Considerations

Enchantum's compile-time reflection process involves scanning a range of potential underlying integer values for enum members. Accurately configuring this search range is important for ensuring all members are correctly discovered and can also influence compile times. Runtime performance for reflected operations is generally efficient.

**Configuring Enum Reflection Scope and Correctness:**

Enchantum determines enum members at compile time by scanning a range of underlying integer values. This range is defined by `ENCHANTUM_MIN_RANGE` (default -256) and `ENCHANTUM_MAX_RANGE` (default 256).

-   **Impact of Value Range**:
    -   If an enum's actual values fall outside this default range, its members may not be fully discovered, leading to incorrect reflection.
    -   If the actual range of values for most of your enums is much smaller than the default (or a globally overridden large range), compile times can be unnecessarily increased as the library scans a wider-than-needed spectrum.

-   **Strong Recommendation: Specialize `enchantum::enum_traits`**:
    To ensure all enum members are found and to manage the scope of the reflection process effectively, it is **strongly recommended** to specialize `enchantum::enum_traits<MyEnum>` for your enums. This is particularly crucial for enums that:
    1.  **Have values outside the default `-256` to `256` range**: Defining `min` and `max` in the traits is necessary to encompass all actual values of your enum.
        ```cpp
        // Example: Enum with values outside the default range
        enum class MyCustomRangeEnum { Val1 = -500, Val2 = 1000 };
        template<> struct enchantum::enum_traits<MyCustomRangeEnum> {
            static constexpr auto min = -500; // Or static_cast<int>(MyCustomRangeEnum::Val1)
            static constexpr auto max = 1000; // Or static_cast<int>(MyCustomRangeEnum::Val2)
        };
        ```
    2.  **Have a known, much smaller range than the default**: Specifying a tight `min` and `max` will reduce the compile-time search space.
        ```cpp
        // Example: Enum with a small, positive range
        enum class MySmallEnum { A = 1, B = 2, C = 3 };
        template<> struct enchantum::enum_traits<MySmallEnum> {
            static constexpr auto min = 1;
            static constexpr auto max = 3;
        };
        ```
-   **Additional Trait: `prefix_length`**:
    `enum_traits` can also be used to specify a `prefix_length` to automatically trim common prefixes from reflected enum names, enhancing the usability of `to_string` and related functions.
    ```cpp
    enum class MyPrefixedEnum { EnumPrefix_OptionA, EnumPrefix_OptionB };
    template<> struct enchantum::enum_traits<MyPrefixedEnum> {
        static constexpr auto prefix_length = sizeof("EnumPrefix_") - 1;
        // If min/max are not specified, they default to ENCHANTUM_MIN_RANGE/MAX_RANGE
    };
    // enchantum::to_string(MyPrefixedEnum::EnumPrefix_OptionA) will yield "OptionA"
    ```

By carefully defining `enum_traits` where appropriate, you provide Enchantum with precise information about your enums. This allows the library to define the correct search space for reflection, which is essential for correctness and can contribute to more efficient compile times.

For more details, see the documentation on [`enum_traits`](docs/features.md#enum_traits) and the discussion on [enum value ranges](docs/limitations.md#enum-range).

---

## Summary

One of the design aims for **enchantum** is to help manage compile times in projects utilizing enum reflection. For example, in one project using enum reflection for configuration, incorporating Enchantum helped lower full rebuild times.

Enchantum requires C++20.
But this requirement can be lifted if there is enough demand for a C++17 version of `enchantum`.

---

## Other Examples
### Checking for validity
```cpp
#include <enchantum/enchantum.hpp>

enum class Direction { North, South, East, West };

enchantum::contains<Direction>("North"); // true
enchantum::contains(Direction(-42));     // false
enchantum::contains<Direction>(0);       // true
```
### Min Max Count
```cpp

enum class Errno { BadSomething = -1, IamGood = 0, IAmBadV2 = 1 };

enchantum::min<Errno>; // BadSomething
enchantum::max<Errno>; //IAmBadV2
enchantum::count<Errno>; // 3
```

---


# CMake Integration

The **cmake** file provides the target `enchantum::enchantum` since this library is header-only it is very simple to use you can copy and paste the files and add the include directory or use **cmake** and the library as a submodule. 

```cpp
add_subdirectory("third_party/enchantum")
target_link_libraries(your_executable enchantum::enchantum)
```