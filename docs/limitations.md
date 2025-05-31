# Limitations

Enchantum requires C++20. While some internal mechanisms might resemble C++17 approaches, the library relies on C++20 features for its public API and full functionality. Support for earlier C++ standards is not currently planned.

This library, similar to some other reflection utilities, utilizes compiler-specific intrinsic macros (such as `__FUNCSIG__` or `__PRETTY_FUNCTION__`) as part of its mechanism to obtain type name information.

## Enum Range

Enum values must be in the range [`ENCHANTUM_MIN_RANGE`,`ENCHANTUM_MAX_RANGE`] 
By default, `ENCHANTUM_MIN_RANGE` = -256, `ENCHANTUM_MAX_RANGE` = 256. 

If you need a different range for all enum types by default, redefine the macro `ENCHANTUM_MAX_RANGE` and if you don't explicitly define `ENCHANTUM_MIN_RANGE` it will be `-ENCHANTUM_MAX_RANGE`. Increasing this range can lead to longer compilation times. For more details on performance characteristics, refer to the [Compile-Time Benchmarks](../README.md#compile-time-benchmarks) section in the README.


*Note: Defining these macros project-wide is recommended if a global adjustment to the default range is needed.*

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

## Unscoped enums in templates may not work correctly on Clang.

Clang exhibits particular behavior with certain unscoped enums defined within templates.

```cpp
template<typename T>
struct ReallyClang_QuestionMark {
    enum Type {
        A,B,C
    };
};

enchantum::entries<ReallyClang_QuestionMark<int>::Type>; // May result in a compiler error on Clang
```

This behavior might be related to how Clang handles or prunes unused type information within templates. A workaround involves ensuring that at least one enumerator of the nested unscoped enum is referenced in a way that its type `decltype(Enum::Enumerator)` is evaluated, for example, by using it in a `using` declaration for the enum type itself:

```cpp
template<typename T>
struct GoodClang {
    enum Type_ {
        A,B,C
    };

    using Type = decltype(Type_::A);
};

enchantum::entries<GoodClang<int>::Type>; // This approach typically resolves the issue on Clang
```

## Unscoped Enums

unnamed unscoped enums are not supported except on msvc

```cpp
enum {
    SomeConstant = 10;
};

using Type = decltype(SomeConstant);
enchantum::entries<Type>; // This is not officially supported and may not work.
```

A workaround for C-style anonymous enums is to provide a name for the enum using `typedef`:
```cpp
typedef enum {
    SomeConstant = 10;
} MyNamedType;

// enchantum::entries<MyNamedType>; // This would then be the type to reflect
```
It is generally recommended to use named enums, whether scoped or unscoped.