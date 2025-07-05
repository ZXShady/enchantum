# Enchantum

**Enchantum** (short for "enchant enum") is a modern **C++20** header-only library for **compile-time enum reflection**. It provides fast, lightweight access to enum values, names, and bitflags all without macros or boilerplate.

> Every year, countless turtles perish due to the pollution caused by slow, bloated build times.  
 Save the turtles — and your compile times — by switching to enchantum!

<sup><sub>Source: I made it up.</sup></sub>

Key Features

  - Macro-free and non-intrusive boilerplate-free reflection

  - Fast compile times (benchmarked below)

  - Most efficient binary size wise.
  
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
      - Scoped Functions
      - Optional null terminator disabling
      - Optional prefix stripping for C-style enums
      - `0` values are reflected for bitflag enums 

[Simple Examples](#simple-examples)

[Why another enum reflection library?](#why-another-enum-reflection-library)

[Features](docs/features.md)

[Benchmarks](#benchmarks)

[Limitations](docs/limitations.md)

[CMake Integration](#cmake-integration)

Tested locally on Windows 10 with:
- Visual Studio 2022 (19.44)
- GCC 14.2.0
- Clang 20.1.2

Compiler Support: (Look at [CI](https://github.com/ZXShady/enchantum/actions))
- GCC >= 11 (GCC 10 Partial no support for enums in templates)
- Clang >= 10
- MSVC >= 19.31 (Tested through godbolt)

---

> [!IMPORTANT]
> Be sure to read [Limitations](docs/limitations.md) before using Enchantum in production.

## Simple Examples

* to string
```cpp
#include <enchantum/enchantum.hpp> // to_string
#include <enchantum/iostream.hpp> // iostream support
enum class Music { Rock, Jazz , Metal };

int main() 
{
  auto music = Music::Rock;
  std::string_view music_name =  enchantum::to_string(music);
  // music_name == "Rock"

  using namespace enchantum::iostream_operators;
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
  for(Music music : enchantum::values_generator<Music>)
    std::cout << static_cast<int>(music) << " ";
  // Prints "0 1 2"

  // Iterate over names
  for(std::string_view name : enchantum::names_generator<Music>)
    std::cout << name << " ";
  // Prints "Rock Jazz Metal"

  // Iterate over both!
  for(const auto [music,name] : enchantum::entries_generator<Music>)
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
- Efficient executable binary size.
- Null terminated strings.

**Cons**
- Compile times grow significantly with larger `MAGIC_ENUM_MAX_RANGE`.

### conjure_enum

**Pros**
- Macro-free (non intrusive)
- Uses C++20

**Cons**

*Note: Could not get this to compile locally. Based on the README, compile times are similar to or worse than magic_enum.*

### simple_enum

**Pros**
- Faster compile times.
- Functor based API.

**Cons**
- Requires specifying enum `first`/`last` values manually (intrusive, doesn't work well with third-party enums)
- Compile time slows down with large enum ranges
- Big binary size bloat.
- No support for bitflags yet.
- No support for null terminated strings.

### enchantum

**Pros**
- Macro-free (non intrusive)
- Allows specifying ranges for specific enums when needed
- Compiles fast.
- Clean and Simple Functor based API `enchantum::to_string(E)` no `enchantum::to_string<E::V>()` since compile times are fast.
- Features like disabling null termination if not needed and specifying common enum prefix for C style enums, and reflect '0' values for bit flag enums.
- Supports all sort of enums (scoped,unscoped,C style unfixed underlying type,anonymous namespaced enums, enums with commas in their typename,etc...);
- Most efficient object binary size and executable size.
- Null terminated strings.

**Cons**
- C++20 required
- Compiler errors are incomprehensible if something goes wrong, needs a level 10 wizard to read them.
- No support for wide strings (yet)

---

## Benchmarks

### Summary

**enchantum** significantly reduces compile times and binary sizes in enum reflection projects. In my own project (which uses [libassert](https://github.com/jeremy-rifkin/libassert) and enum reflection for configuration), switching from `magic_enum` reduced full rebuild times from about 2 minutes to 1 minute and 26 seconds (34 seconds difference).
I also tried compiling my project using [-2048,2048] as my range and it took 1 minute and 46 seconds! that's still less than `magic_enum` by default while having **16x** the default range.
The trade-off is that `enchantum` requires C++20, while `magic_enum` supports C++17.
But this requirement can be lifted if there is enough demand for a C++17 version of `enchantum`.


Each compile time benchmark was run 10 times and averaged unless noted otherwise.
`range` is `ENCHANTUM_MAX_RANGE` and `MAGIC_ENUM_RANGE_MAX`, for `simple_enum` it is defining `last` and `first` with the range because I could not find a macro for this, this is technically misuse of the library since it likes having these values close to the actual range but the comparisons would be unfair.

The enum members are from 0 to Count


| Test Case       | Small      | Big          | Large Range | Ideal Range |
|-----------------|------------|--------------|-------------|-------------|
| Number of Enums | 200        | 32           | 200         | 100         |
| Enum Range      | (-128,128) | (-256,256)   | (-1024,1024)| (0,50)      |
| Values per Enum | 16         | 200          | 16          |  50         |


### Compile Time
All times in seconds (lower is better). Compiled with `-O3` fir GCC and Clang while `/Ox` for MSVC. 

"Timeout" means it took more than 20 minutes and still did not finish

| Compiler    | Test Case   | `enchantum`  | `magic_enum` | `simple_enum` |
|-------------|-------------|--------------| ------------ |---------------|
| **GCC**     | Small       | 6.1          |  47          | 21.5          |
|             | Big         | 4.5          |  21          | 6.3           |
|             | Large Range | 26.7         |  Timeout     | 313           |
|             | Ideal Range | 3            |  8.1         | 2.7           |
|                                                                         |
| **Clang**   | Small       | 5.6          |  47          | 14            |
|             | Big         | 2.3          |  18          | 4.4           |
|             | Large Range | 14.8         |  Timeout     | 96.3          |
|             | Ideal Range | 2.9          |  8.7         | 2.3           |
|                                                                         |
| **MSVC**    | Small       | 15.8         |  80          | 186           |
|             | Big         | 8.8          |  37          | 32.1          |
|             | Large Range | 85.3         |  Timeout     | Timeout       |
|             | Ideal Range | 5.8          |  17.9        | 4.7           |

## Object File Sizes

Lower is better, all measurements are in kilobytes.

[A simple godbolt link for magic_enum vs enchantum](https://godbolt.org/#g:!((g:!((h:output,i:(editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86-64+clang+(trunk)+(Compiler+%231)',t:'0'),(h:compiler,i:(compiler:clang_trunk,filters:(b:'0',binary:'1',binaryObject:'1',commentOnly:'0',debugCalls:'1',demangle:'0',directives:'0',execute:'1',intel:'0',libraryCode:'0',trim:'1',verboseDemangling:'0'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-O3+-DENCH%3D1+-std%3Dc%2B%2B20',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+clang+(trunk)+(Editor+%231)',t:'0'),(h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:13,endLineNumber:10,positionColumn:13,positionLineNumber:10,selectionStartColumn:13,selectionStartLineNumber:10,startColumn:13,startLineNumber:10),source:'%23if+ENCH%0A%23include+%3Chttps://raw.githubusercontent.com/ZXShady/enchantum/refs/heads/main/single_include/enchantum_single_header.hpp%3E%0A%23define+magic_enum+enchantum%0A%23define+enum_name+to_string%0A%23else%0A%23include+%3Chttps://raw.githubusercontent.com/Neargye/magic_enum/refs/heads/master/include/magic_enum/magic_enum.hpp%3E%0A%23endif%0Aenum+class+A+%7Balong,b,c,e,d,f,glong%7D%3B%0Aextern+A+enm%3B%0Aextern+const+void*+volatile+p%3B%0Aint+main()+%7B%0A++++p+%3D+magic_enum::enum_name(enm).data()%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),header:(),l:'4',m:100,n:'0',o:'',s:2,t:'0')),version:4)


Clang is only currently measured.

| Compiler    | Test Case   | `enchantum`  | `magic_enum` | `simple_enum` |
|-------------|-------------|--------------| ------------ |---------------|
| **Clang**   | Small       | 275          | 732          | 12070         |
|             | Big         | 84           | 1307         | 3847          |
|             | Large Range | 275          | Unknown      | 98366         |
|             | Ideal Range | 299          | 1051         | 1233          |


**Note**:
The reason `simple_enum` is so big in object sizes is that it generates a huge table of all strings 
it does not do string optimizations like `enchantum` or `magic_enum` which leads to big object sizes

It stores this for example `"auto se::f() [enumeration = A_0::A_0_4]"`, the entire internal string instead of 
only the part it needs (the "A_0_4" part)
and this gets worse as the main enum name gets longer (e.g by putting the enums in a namespace), 
but it also allows the library to do `O(1)` enum to string lookup for **any** enum, 
which I don't necessarily think is worth it and compile faster.


### Executable Sizes

Compiled the object files into their own executable.
Lower is better, all measurements are in kilobytes.

| Compiler    | Test Case   | enchantum     | magic_enum     | simple_enum     |
|-------------|-------------|---------------|----------------|-----------------|
| **Clang**   | Small       | 40            | 95             | 2897            |
|             | Big         | 56            | 168            | 944             |
|             | Large Range | 40            | Unknown        | 23200           |
|             | Ideal Range | 46            | 134            | 308             |




---



# CMake Integration

The **cmake** file provides the target `enchantum::enchantum` since this library is header-only it is very simple to use you can copy and paste the files and add the include directory or use **cmake** and the library as a submodule. 

```cpp
add_subdirectory("third_party/enchantum")
target_link_libraries(your_executable enchantum::enchantum)
```