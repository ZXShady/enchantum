# Enchantum

**Enchantum** (short for "enchant enum") is a modern **C++20** header-only library for **compile-time enum reflection**. It provides fast, lightweight access to enum values, names, and bitflags all without macros or boilerplate.

> Every year, countless turtles perish due to the pollution caused by slow, bloated build times.  
 Save the turtles — and your compile times — by switching to enchantum!

<sup><sub>Source: I made it up.</sup></sub>

Key Features

  - Macro-free and non-intrusive boilerplate-free reflection

  - Fast compile times (benchmarked below)

  - Supports:
       -  Scoped enums
       -  Unscoped C enums
       -  Sparse enums
       -  Bitflags
       -  Iteration over values,names and `[enum, name]` pairs.
       -  `string <=> enum` conversions
       -  `enum <=> index` conversions
       -  enum validation functions like `cast` and `contains`
       -  enum aware containers: `enchantum::bitset` and `enchantum::array`
  - Extra features like:
      - Optional null terminator disabling
      - Optional prefix stripping for C-style enums
      - `0` values are reflected for bitflag enums 

[Simple Examples](#simple-examples)

[Why another enum reflection library?](#why-another-enum-reflection-library)

[Features](docs/features.md)

[Compile Time Benchmarks](#compile-time-benchmarks)

[Limitations](docs/limitations.md)

[CMake Integration](#cmake-integration)

Tested locally on Windows 10 with:
- Visual Studio 2022 (v17.13.6)
- GCC 14.2.0
- Clang 20.1.2

Compiler Support: (Look at [CI](https://github.com/ZXShady/enchantum/actions))
- GCC >= 11 (GCC 10 Partial no support for enums in templates)
- Clang >= 13

---

> [!IMPORTANT]
> Be sure to read [Limitations](docs/limitations.md) before using Enchantum in production.

## Simple Examples

Enums used in the examples

* to string
```cpp
#include <enchantum/enchantum.hpp> // to_string
#include <enchantum/ostream.hpp> // ostream support
enum class Music { Rock, Jazz , Metal };

int main() 
{
  auto music = Music::Rock;
  std::string_view music_name =  enchantum::to_string(music);
  // music_name == "Rock"

  using namespace enchantum::ostream_operators;
  std::cout << music;
  // Prints Rock
}
```

* from strings
```cpp
#include <enchantum/enchantum.hpp> // cast
enum class Music { Rock, Jazz , Metal };
int main() 
{
  // case sensitive
  std::optional<Music> music = enchantum::cast<Music>("Jazz");
  if(music.has_value()) // check if cast succeeded
  {
    // *music == Music::Jazz
  }
  // pass a predicate taking two string views
  music = enchantum::cast<Music>("JAZZ",[](std::string_view a,std::string_view b){
    return std::ranges::equal(a,b,[](unsigned char x,unsigned char y){
      return std::tolower(x) == std::tolower(y);      
    });
  });
  if(music.has_value()) {
      // *music == Music::Jazz
  }
}
```

* index into enums
```cpp
#include <enchantum/enchantum.hpp> // index_to_enum and enum_to_index
enum class Music { Rock, Jazz , Metal };

int main() 
{
  // case sensitive
  std::optional<Music> music = enchantum::index_to_enum<Music>(1); // Jazz is the second enum member
  if(music.has_value()) // check if index is not out of bounds
  {
    // *music == Music::Jazz
    std::optional<std::size_t> index = enchantum::enum_to_index(*music);
    // *index == 1
  }
}
```

* iteration
```cpp
#include <enchantum/enchantum.hpp> // entries,values and names 
enum class Music { Rock, Jazz , Metal };

int main() 
{
  // Iterate over values
  for(Music music : enchantum::values<Music>)
    std::cout << static_cast<int>(music) << " ";
  // Prints "0 1 2"

  // Iterate over names
  for(std::string_view name : enchantum::names<Music>)
    std::cout << name << " ";
  // Prints "Rock Jazz Metal"

  // Iterate over both!
  for(const auto& [music,name] : enchantum::entries<Music>)
    std::cout << name << " = " << static_cast<int>(music) << "\n";
  // Prints 
  // Rock = 0
  // Jazz = 1
  // Metal = 2
}
```

Look at [Features](docs/features.md) for more information.


## Why Another Enum Reflection Library?

There are several enum reflection libraries out there — so why choose **enchantum** instead of [magic_enum](https://github.com/Neargye/magic_enum), [simple_enum](https://github.com/arturbac/simple_enum), or [conjure_enum](https://github.com/fix8mt/conjure_enum)?


### magic_enum

**Pros**
- Macro-free (non intrusive).
- No modifications needed for existing enums.
- Allows specifying ranges for specific enums when needed.
- Supports C++17.
- Nicer compiler errors.
- Supports wide strings.
- Efficient Binary size.

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
- Fast compile times but only if range is small.
- Functor based api.

**Cons**
- Requires specifying enum `first`/`last` values manually (intrusive, doesn't work well with third-party enums)
- Compile time slows down with large enum ranges
- Binary Size Bloat.

### enchantum

**Pros**
- Macro-free (non intrusive)
- Does not sacrifice API ease of use and features for compile time sake (e.g no `ENCHANTUM_ALL_ENUMS_ARE_CONTIGUOUS_OPTIMIZATION` flag which would disable support for sparse enums)
- Allows specifying ranges for specific enums when needed
- Compiles fast.
- Clean and Simple Functor based API `enchantum::to_string(E)` no `enchantum::to_string<E::V>()` since compile times are fast.
- Features like disabling null termination if not needed and specifying common enum prefix for C style enums, and reflect '0' values for bit flag enums.
- Supports all sort of enums (scoped,unscoped,C style unfixed underlying type,anonymous namespaced enums, enums with commas in their typename,etc...);
- Efficient Binary size

**Cons**
- C++20 required
- Compiler errors are incomprehensible if something goes wrong, needs a level 10 wizard to read them.
- No support for wide strings (yet)
---

## Compile-Time Benchmarks

Each benchmark was run 10 times (except MSVC which was ran 3 times) and averaged unless noted otherwise.
`range` is `ENCHANTUM_MAX_RANGE` and `MAGIC_ENUM_RANGE_MAX`
### Small Enums  
enum count: `200`

enum member count: `16`

range: `-128` to `128`

| Compiler    | `magic_enum`     | `enchantum`     | Time Saved |
| ----------- | ---------------- | --------------- | ---------- |
| MSVC        | 80 secs          | 22  secs        | 58         |
| GCC         | 47 secs          | 6.1 secs        | 40.9       |
| Clang       | 47 secs          | 6.2 secs        | 40.8       |

### Large Enums  
enum count: `32`

enum member count: `200`

range: `-256` to `256`


| Compiler    | `magic_enum`     | `enchantum`     | Time Saved |
| ----------- | ---------------- | --------------- | ---------- |
| MSVC        | 37 secs          | 15  secs        | 22         |
| GCC         | 21 secs          | 4.5 secs        | 16.5       |
| Clang       | 18 secs          | 5 secs          | 13         |


### Very Large Enum Range  
enum count: `200`

enum member count: `16`

range: `-1024` to `1024`

*Only ran 2 times due to extreme compilation times.*

| Compiler | `magic_enum`     | `enchantum`   |
|----------|------------------|---------------|
| MSVC     | >20 min (killed) | 107  secs     |
| GCC      | >15 min (killed) | 26.7 secs     |
| Clang    | >15 min (killed) | 24.5 secs     |
---

## Summary

**enchantum** significantly reduces compile times in enum reflection projects. In my own project (which uses [libassert](https://github.com/jeremy-rifkin/libassert) and enum reflection for configuration), switching from `magic_enum` reduced full rebuild times from about 2 minutes to 1 minute and 30 seconds. I was surprised that `magic_enum` alone took 30 seconds.

The trade-off is that `enchantum` requires C++20, while `magic_enum` supports C++17.
But this requirement can be lifted if there is enough demand for a C++17 version of `enchantum`.


# CMake Integration

The **cmake** file provides the target `enchantum::enchantum` since this library is header-only it is very simple to use you can copy and paste the files and add the include directory or use **cmake** and the library as a submodule. 

```cpp
add_subdirectory("third_party/enchantum")
target_link_libraries(your_executable enchantum::enchantum)
```