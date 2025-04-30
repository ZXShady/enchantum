# Limitations

This library requires C++20. but I can make a C++17 version if really wanted the clang implementation is actually almost C++17 except for some things.

This library just like other libraries uses compiler-specific hacks based on `__FUNCSIG__` / `__PRETTY_FUNCTION__`.

## Enum Range

Enum values must be in the range [`ENCHANTUM_MIN_RANGE`,`ENCHANTUM_MAX_RANGE`] 
By default, `ENCHANTUM_MIN_RANGE` = -256, `ENCHANTUM_MAX_RANGE` = 256. which is twice of that of `magic_enum`'s [`-128`,`127`]

If you need a different range for all enum types by default, redefine the macro `ENCHANTUM_MAX_RANGE` and if you don't explicitly define `ENCHANTUM_MIN_RANGE` it will be `-ENCHANTUM_MAX_RANGE`. Increasing this value can lead to longer compilation times but unlike `magic_enum` it is not massivly increasing [see benchmarks](../README.md#compile-time-benchmarks).

```cpp
#define ENCHANTUM_MIN_RANGE 0 // if not defined it will be -512
#define ENCHANTUM_MAX_RANGE 512
#include <enchantum/enchantum.hpp>
```

If you need a different range for a specific enum type, add the specialization `enum_traits` for the enum type.

```cpp
#include <enchantum/enchantum.hpp>

enum class wizards { longbeard = 300, midbeard = 400, redbearded = 600 };


template <> 
struct enchantum::enum_traits<wizards> { // defined in enchantum/common.hpp you only need that header
  static constexpr auto min = static_cast<int>(wizards::longbeard);
  static constexpr auto max = static_cast<int>(wizards::redbearded);
};
```

## Other Compiler Issues

If you see a message that goes like this

```
note: constexpr evaluation hit maximum step limit; possible infinite loop? // gcc or clang
note: maximum constexpr step count exceeded. // msvc
```

Change the limit for the number of constexpr steps allowed: (hyperlink to docs)

[MSVC](https://docs.microsoft.com/en-us/cpp/build/reference/constexpr-control-constexpr-evaluation): `/constexpr:depthN, /constexpr:stepsN`

[Clang](https://clang.llvm.org/docs/UsersManual.html#controlling-implementation-limits): `-fconstexpr-depth=N, -fconstexpr-steps=N`

[GCC](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-fconstexpr-depth): `-fconstexpr-depth=N, -fconstexpr-loop-limit=N, -fconstexpr-ops-limit=N`

## Unscoped enums in templates may not work correctly on Сlang.

Clang is super weird with these enums

```cpp
template<typename T>
struct ReallyClang_QuestionMark {
    enum Type {
        A,B,C
    };
};

enchantum::entries<ReallyClang_QuestionMark<int>::Type>; // some long compiler error
```

Apparantly Clang optimizes unused  enum names in templates? I don't know really but a workaround is this (you must atleast mentioning the name of an enumarator once)

```cpp
template<typename T>
struct GoodClang {
    enum Type_ {
        A,B,C
    };

    using Type = decltype(Type_::A);
};

enchantum::entries<GoodClang<int>::Type>; // happy clang
```

## Unscoped Enums

unnamed unscoped enums are not supported except on msvc

```cpp
enum {
    SomeConstant = 10;
};

using Type = decltype(SomeConstant);
enchantum::entries<Type>; // won't work and won't be officially supported
```

this works

```cpp
typedef enum {
    SomeConstant = 10;
} Type;
```

just give it a name.