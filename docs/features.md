All the functions/variables are defined in namespace `enchantum`

All non-`void` functions are `[[nodiscard]]` unless explicitly said otherwise.

**Note**: Documentation Incomplete.

Quick Reference

**Concepts And Traits**:
  - [Enum](#enum)
  - [SignedEnum](#signedenum)
  - [UnsignedEnum](#unsignedenum)
  - [ScopedEnum](#scopedenum)
  - [UnscopedEnum](#unscopedenum)
  - [ContiguousEnum](#contiguousenum)
  - [BitFlagEnum](#bitflagenum)
  - [ContiguousBitFlagEnum](#contiguousbitflagenum)
  - [EnumOfUnderlying](#enumofunderlying)
  - [EnumFixedUnderlying](#enumfixedunderlying)
  - [has_zero_flag](#has_zero_flag)
  - [is_contiguous](#is_contiguous)
  - [is_bitflag](#is_bitflag)
  - [is_contiguous_bitflag](#is_contiguous_bitflag)
  - [enum_traits](#enum_traits)

**Functions**:
  - [to_string](#to_string)
  - [to_string_bitflag](#to_string_bitflag)
  - [cast](#cast)
  - [cast_bitflag](#cast_bitflag)
  - [contains](#contains)
  - [contains_bitflag](#contains_bitflag)
  - [scoped::*](#scoped-functions)
  - [index_to_enum](#index_to_enum)
  - [enum_to_index](#enum_to_index)
  - [next_value/prev_value](#next_valueprev_value)
  - [std::format/fmt::format](#fmtformat--stdformat-support)
  - [iostream support](#operator-stream-output-operator)
  - [for_each](#for_each)
  - [to_underlying](#to_underlying)

**Constants**:
  - [entries](#entries)
  - [names](#names)
  - [values](#values)
  - [entries_generator](#entries_generator)
  - [names_generator](#names_generator)
  - [values_generator](#values_generator)
  - [max](#max)
  - [min](#min)
  - [count](#count)
  - [value_ors](#value_ors)
  - [type_name](#type_name)
  - [raw_type_name](#raw_type_name)
  
**Containers**:
  - [array](#array)
  - [bitset](#bitset)

**Macros**:
  - [ENCHANTUM_DEFINE_BITWISE_FOR](#enchantum_define_bitwise_for)
  - [ENCHANTUM_ASSERT](#enchantum_assert)
  - [ENCHANTUM_THROW](#enchantum_throw)
  - [ENCHANTUM_ENABLE_MSVC_SPEEDUP](#enchantum_enable_msvc_speedup)
  - [ENCHANTUM_OPTIONAL](#enchantum_optional)
  - [ENCHANTUM_STRING](#enchantum_string)
  - [ENCHANTUM_STRING_VIEW](#enchantum_string_view)


# Concepts
## Enum
The Enum concept is used to identify types that are valid C++ enums. Any type that is an enum, whether scoped or unscoped, signed or unsigned, will satisfy this concept.

```cpp
template<typename T>
concept Enum = std::is_enum_v<T>;
```

> Example usage:
```cpp
#include <enchantum/common.hpp>

enum class Color { Red, Green, Blue };
static_assert(enchantum::Enum<Color>);
```

## SignedEnum

The SignedEnum concept restricts types to enums whose underlying type is a signed integral type. It ensures that only enums with a signed integral base type (e.g., int, short) are used.

```cpp
#include <enchantum/common.hpp>

template<typename T>
concept SignedEnum = Enum<T> && std::signed_integral<std::underlying_type_t<T>>;
```

> Example usage:

```cpp
#include <enchantum/common.hpp>

enum class SignedColor : int { Red = -1, Green, Blue };
static_assert(enchantum::SignedEnum<SignedColor>);
```

## UnsignedEnum

The UnsignedEnum concept restricts types to enums whose underlying type is an unsigned integral type. It ensures that only enums with an unsigned integral base type (e.g., unsigned int, unsigned char) are used.

**Note** `bool` is considered an `unsigned` type
```cpp
#include <enchantum/common.hpp>

template<typename T>
concept UnsignedEnum = Enum<T> && !SignedEnum<T>;
```

> Example usage:

```cpp
#include <enchantum/common.hpp>

enum class UnsignedColor : unsigned int { Red, Green, Blue };
static_assert(enchantum::UnsignedEnum<UnsignedColor>);
```

## ScopedEnum
The ScopedEnum concept restricts to enums that are scoped (i.e., they do not implicitly convert to their underlying type). This applies to enum class in C++.

```cpp
template<typename T>
concept ScopedEnum = Enum<T> && (!std::is_convertible_v<T, std::underlying_type_t<T>>);
```

> Example usage:

```cpp
#include <enchantum/common.hpp>

enum class ScopedColor { Red, Green, Blue };
static_assert(enchantum::ScopedEnum<ScopedColor>);
```

## UnscopedEnum
The UnscopedEnum concept restricts to enums that are unscoped (i.e., they implicitly convert to their underlying type). This applies to traditional C-style enums.

```cpp
template<typename T>
concept UnscopedEnum = Enum<T> && !ScopedEnum<T>;
```

> Example usage:

```cpp
#include <enchantum/common.hpp>

enum UnscopedColor { Red, Green, Blue };
static_assert(enchantum::UnscopedEnum<UnscopedColor>);
```

## BitFlagEnum
The `BitFlagEnum` concept is used for enums that support main bitwise operations, such as `&`, `|` , `~` , `&=` and `|=` . This concept can be used to check if an enum is intended for bitflag operations, where each enum value represents a distinct bit.

Enums that satify this concept have their 2^N members and 0 value reflected.

```cpp
#include <enchantum/common.hpp>

template<typename T>
concept BitFlagEnum = Enum<T> && is_bitflag<T>;
```

> Example usage:
```cpp
#include <enchantum/common.hpp>

enum class Flags : uint32_t {
    None = 0,
    FlagA = 1 << 0,
    FlagB = 1 << 1,
    FlagC = 1 << 2,
    FlagA_B = FlagA | FlagB // not reflected only 2^N members are reflected
};

Flags operator&(Flags a,Flags b); // can return bool
Flags operator|(Flags a,Flags b);
Flags operator~(Flags x);
Flags& operator&=(Flags a,Flags b);
Flags& operator|=(Flags a,Flags b);


static_assert(enchantum::BitFlagEnum<Flags>);
```

### ContiguousBitFlagEnum
The `ContiguousBitFlagEnum` concept subsumes `BitFlagEnum` and checks whether the flag values are contiguous.

```cpp
template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;
```

> Example:

```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitwise_operators.hpp>

enum class ContiguousFlags : unsigned int {
    None = 0,
    OptA = 1 << 0,
    OptB = 1 << 1,
    OptC = 1 << 2 
};
ENCHANTUM_DEFINE_BITWISE_FOR(ContiguousFlags) 

static_assert(enchantum::ContiguousBitFlagEnum<ContiguousFlags>);

enum class NonContiguousFlags : unsigned int {
    None = 0,
    OptA = 1 << 0,
    OptB = 1 << 2, // skipped  1 << 1
    OptC = 1 << 3 
};
ENCHANTUM_DEFINE_BITWISE_FOR(NonContiguousFlags)

static_assert(!enchantum::ContiguousBitFlagEnum<NonContiguousFlags>);

```

### `ContiguousEnum`

The ContiguousEnum concept is used for enums where the underlying values are contiguous. For example, `enum { A = 0, B, C }` is a contiguous enum because the underlying values are 0, 1, and 2, respectively.

```cpp
template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;
```

> Example usage:
```cpp
#include <enchantum/common.hpp>

enum class Status { Ok = 0, Error = 1, Unknown = 2 };
static_assert(enchantum::ContiguousEnum<Status>);
```

### `EnumOfUnderlying`

```cpp
// defined in header `common.hpp`

template<typename E, typename Underlying>
concept EnumOfUnderlying = Enum<E> && std::same_as<std::underlying_type_t<E>, Underlying>;
```

constrains an enum to a specific underlying type.

> Example usage:
```cpp
#include <enchantum/common.hpp>

enum class Status : char { Ok = 0, Error = 1, Unknown = 2 };
static_assert(enchantum::EnumOfUnderlying<Status,char>);
```

### `EnumFixedUnderlying`

```cpp
// defined in header `common.hpp`

template<typename T>
concept EnumFixedUnderlying = Enum<T> && requires { T{0}; };
```
- **Description**:
  Checks whether the enum has an explicit underlying type specified.

> Example usage:
```cpp
#include <enchantum/common.hpp>

// enum classes default to int
enum class Status {};
enum Status2 {}; // no fixed underlying

static_assert(enchantum::EnumFixedUnderlying<Status>);
static_assert(!enchantum::EnumFixedUnderlying<Status2>);
```


### `has_zero_flag`

```cpp
// defined in header `enchantum.hpp`

template<typename>
inline constexpr bool has_zero_flag = false;

template<BitFlagEnum E>
inline constexpr bool has_zero_flag<E> = /*impl*/;

```
**Description**:
  Checks whether a `BitFlagEnum` has a zero flag (i.e `None` value)

> Example usage:
```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitwise_operators.hpp>
enum class FlagsWithoutNone : std::uint8_t {
  A = 1 << 0,
  B = 1 << 1
};
ENCHANTUM_DEFINE_BITWISE_FOR(FlagsWithoutNone)

enum class FlagsWithNone : std::uint8_t {
  Nothing = 0,
  A = 1 << 0,
  B = 1 << 1
};
ENCHANTUM_DEFINE_BITWISE_FOR(FlagsWithNone)

static_assert(!enchantum::has_zero_flag<FlagsWithoutNone>);
static_assert(enchantum::has_zero_flag<FlagsWithNone>);
```
---

### `enum_traits`

```cpp
// defined in header `common.hpp`

template<typename T>
struct enum_traits;

template<Enum E>
struct enum_traits<E> // default
{
  constexpr static auto prefix_length = 0;
  constexpr static auto min = ENCHANTUM_MIN_RANGE;
  constexpr static auto max = ENCHANTUM_MAX_RANGE;
};
```
- **Description**:
  A customization point for `enum`s by this library like setting `prefix_length`,`min`,`max` instead of using default values.

- **Notes**:
  defining `prefix_length` is not required not defining it defaults to `0`.

> Example usage:
```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitwise_operators.hpp>
// this is a big enum it is outside of default range [-256,256] and it has this annoying prefix
enum BigEnumOutsideOfDefault : std::uint16_t {
  BigEnumOutsideOfDefault_A = 0,
  BigEnumOutsideOfDefault_B = 4096 
};

template<>
struct enchantum::enum_traits<BigEnumOutsideOfDefault> {
  constexpr static auto prefix_length = sizeof("BigEnumOutsideOfDefault_")-1; // sizeof includes null terminator!
  constexpr static auto min = 0;    // inclusive
  constexpr static auto max = 4096; // inclusive
};


// prefix removed
constexpr auto& names = enchantum::names<BigEnumOutsideOfDefault>;
static_assert(names[0] == "A");
static_assert(names[1] == "B");
```

--- 
## Functions

#### `to_string`
```cpp
// defined in header enchantum.hpp

namespace details 
{
  struct TO_STRING_FUNCTOR {
    template<Enum E>
    constexpr std::string_view operator()(E value) const noexcept;
  };
}

constexpr inline details::TO_STRING_FUNCTOR to_string;
```

**Description**:
  Converts an enum value to its corresponding string representation. 
  **Note** the `std::string_view` points to null-terminated character array. 
  and the "function" is a functor allowing easy passing of higher order functions without manually specifying the enum type could be handy in `std::ranges`.

**Notes**:
  Additional overloads may be provided to optimize for specific properties (e.g enums with no gaps can use an index lookup)

**Parameters**:  
  - `value`: The enum value you want to convert to a string.

**Returns**:  
  A `std::string_view` representing the name of the enum value. If no matching string is found, an empty `std::string_view` is returned.

**Example**:
  ```cpp
  enum class Color { Red, Green, Blue };

  Color c = Color::Green;
  std::string_view colorName = enchantum::to_string(c);
  std::cout << colorName << std::endl;  // Outputs: "Green"
  std::string_view invalidColorName = enchantum::to_string(Color(42));
  std::cout << invalidColorName << std::endl;  // Outputs: ""
  ```

#### `to_string_bitflag`
```cpp
// defined in header bitflags.hpp

template<typename String = std::string, BitFlagEnum E>
constexpr String to_string_bitflag(E value, char sep = '|');

```

**Description**:
   Converts a bitflag enum value into a delimited string representation. Each flag set in the input value is converted to its corresponding name and joined by a separator character.
**Notes**:
    If the value contains bits that are not part of any valid enum flag (i.e. not in `values<E>`), a default constructed `String` is returned.
    The separator can be customized (defaults to '|') and the string used.
    The return type defaults to `std::string` but can be customized via the String template parameter.
    The string bitflag order is defined by ascending order. 

**Parameters**:  
  - `value`: The bitflag enum value you want to convert to a string.

**Example**:
```cpp
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/enchantum.hpp>
#include <cstddef>

enum class Flags : std::uint8_t {
    None = 0,
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2
};
ENCHANTUM_DEFINE_BITWISE_FOR(Flags)

std::cout << enchantum::to_string_bitflag(Flags::C | Flags::A,',');  
// Outputs: "A,C" since C is greater than A

std::cout << enchantum::to_string_bitflag(static_cast<Flags>(8));  
// Outputs: "" (invalid combination)
```

### `cast`

```cpp
// defined in header `enchantum.hpp`

namespace details 
{
  template<Enum E>
  struct CAST_FUNCTOR {
  constexpr std::optional<E> operator()(std::underlying_type_t<E> value) noexcept;

  constexpr std::optional<E> operator()(std::string_view name) noexcept;

  template<std::predicate<std::string_view, std::string_view> BinaryPred>
  constexpr std::optional<E> operator()(std::string_view name, BinaryPred binary_predicate) const noexcept
  };
}

template<Enum E>
constexpr inline details::CAST_FUNCTOR<E> cast;

```
**Description**:

 1. Attempts to convert an integral underlying type value to the corresponding enum value if it is a valid underlying value (i.e it is one of the elements of `values<E>`).
 2. Attempts to convert a `std::string_view` to the corresponding enum value based on its name it is case sensitive.
 3. Attempts to convert a `std::string_view` to the corresponding enum value based using a predicate. **note** the first arguement of the predicate is the `cast` `name` arguement while the second arguement of the predicate is the names of the enum of `names<E>`.

Returns:
1. An `optional<E>` containing the enum value if the integer matches a defined enum value; otherwise, `std::nullopt`.
2. An `optional<E>` containing the enum value if the `name` matches a defined enum name; otherwise, `std::nullopt`.
3. Same as 2. but if `binary_predicate` returns `true`

**Examples**:
```cpp
#include <enchantum/enchantum.hpp>

enum class Status { Ok = 0, Error = 1, Unknown = 2 };
assert(enchantum::cast<Status>(1).has_value());
assert(!enchantum::cast<Status>(300).has_value());

assert(enchantum::cast<Status>("Unknown").has_value());

assert(!enchantum::cast<Status>("UnKnoWn").has_value());

assert(enchantum::cast<Status>("UnKnOwn",[](std::string_view a,std::string_view b){
  return std::ranges::equal(a,b,[](unsigned char x,unsigned char y){
    return std::tolower(x) == std::tolower(y);
  })
}).has_value());

```

### `cast_bitflag`

```cpp
// defined in header bitflags.hpp

template<BitFlagEnum E, std::predicate<std::string_view, std::string_view> BinaryPred>
constexpr std::optional<E> cast_bitflag(std::string_view name, char sep, BinaryPred binary_pred) noexcept;

template<BitFlagEnum E>
constexpr std::optional<E> cast_bitflag(std::string_view name, char sep = '|') noexcept;

template<BitFlagEnum E>
constexpr std::optional<E> cast_bitflag(E value) noexcept;
```
**Description**:
These functions attempt to convert a delimited string or a raw enum value into a valid BitFlagEnum value.

1. String to Bitflag (custom comparator)
    Parses a delimited string of flag names and combines the corresponding enum values. A custom binary predicate is used to compare string segments.

2. String to Bitflag
    Same as (1) but uses == for name comparison.

3. Enum value safe cast
    Validates whether the given BitFlagEnum value consists only of known valid flag combinations.

**Parameters**:

`name`: The string containing flag names, separated by the sep character.

`sep`: A character used to delimit individual flag names (default: '|').

`binary_pred`: A predicate to compare the input string segments to the enum `names<E>`.

`value`: An enum value to validate as a valid bitflag combination.

**Returns**:

An `std::optional<E>` containing the constructed bitflag enum if valid; otherwise, `std::nullopt`.

**Example**:
```cpp 
#include <enchantum/enchantum.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <cassert>

enum class Permissions : std::uint8_t {
  None = 0, // this is reflected
  Read = 1 << 0,
  Write = 1 << 1,
  Execute = 1 << 2
};
ENCHANTUM_DEFINE_BITWISE_FOR(Permissions)

std::optional<Permissions> p = enchantum::cast_bitflag<Permissions>("Write|Read");
assert(p == (Permissions::Read | Permissions::Write));

// Case-insensitive parsing
std::optional<Permissions> ci = enchantum::cast_bitflag<Permissions>("read|WRITE", '|',
  [](std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](unsigned char x, unsigned char y) {
      return std::tolower(x) == std::tolower(y);
    });
  }
);
assert(ci == (Permissions::Read | Permissions::Write));

assert(enchantum::cast_bitflag(Permissions::Read | Permissions::Execute).has_value());

assert(!enchantum::cast_bitflag(static_cast<Permissions>(1 << 3)).has_value());
```

### `min`

```cpp
template<Enum E>
inline constexpr E min;
```

**Description**:  
  Gives the minimum enum value

**Example**:
  ```cpp
  enum class Status { Ok = -1, Error = 1, Unknown = 2 };

  auto minValue = enchantum::min<Status>; // Status::Ok
  std::cout << static_cast<int>(minValue) << std::endl;  // Outputs: -1
  ```

---

### `max`

```cpp
template<Enum E>
inline constexpr E max;
```

**Description**:  
  Gives the maximum enum value

**Example**:
```cpp
#include <enchantum/enchantum.hpp>

enum class Status { Ok = -1, Error = 1, Unknown = 53 };

auto maxValue = enchantum::max<Status>; // Status::Unknown
std::cout << static_cast<int>(maxValue) << std::endl;  // Outputs: 53
```

---

### `value_ors`

```cpp
// defined in header `bitflags.hpp`

template<BitFlagEnum E>
inline constexpr E value_ors;
```

**Description**:  
  Gives the bitwise `or`ing of enum values.

**Example**:
```cpp
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/bitflags.hpp>
#include <cstdint>

enum class Flags : std::uint8_t {
  None  = 0,
  Flag1 = 1 << 0,  // 1
  Flag2 = 1 << 1,  // 2
  Flag3 = 1 << 2,  // 4
  Flag4 = 1 << 3,  // 8
};
ENCHANTUM_DEFINE_BITWISE_FOR(Flags)

int main() {
  std::cout << "Combined flags value: " << static_cast<int>(enchantum::value_ors<Flags>) << std::endl;
  // Outputs: 15 (1 | 2 | 4 | 8)
  return 0;
}
```

### `type_name`

```cpp
// defined in header `type_name.hpp`

template<typename T>
constexpr inline std::string_view type_name = /*implementation detail";
```

**Description**:  
  Gives an implementation defined null-terminated string representing the type name.

**Notes**: `T` must not be a function, function pointer, member function pointer or a templated type otherwise a `static_assert` fires.
  

**Example**:
```cpp
#include <enchantum/type_name.hpp>

enum class Enum : std::uint8_t;

namespace NS { struct Type; }

enchantum::type_name<Enum>;
enchantum::type_name<NS::Type>; // ignores namespaces
// Gives on ALL compilers:
// "Enum"
// "Type"
// Other output may be compiler dependant
```

### `raw_type_name`

```cpp
// defined in header `type_name.hpp`

template<typename T>
constexpr inline std::string_view raw_type_name = /*implementation detail";
```

**Description**:  
  Gives an implementation defined null-terminated string representing the type name.

**Example**:
```cpp
#include <enchantum/type_name.hpp>

enum class Enum : std::uint8_t;

namespace NS { struct Type; }

enchantum::raw_type_name<Enum>;
enchantum::raw_type_name<NS::Type>;
// Gives on ALL compilers:
// "Enum"
// "NS::Type"
// Other output may be compiler dependant
```


---

### `count`

```cpp
template<Enum E>
inline constexpr std::size_t count;
```

**Description**:  
  Gives the number of enum values in the enum type.

**Example**:
  ```cpp
  enum class Status { Ok = -1, Error = 51, Unknown = 2 };
  std::cout << enchantum::count<Status> << std::endl;  // Outputs: 3
  ```

---

### `entries`

```cpp
// defined in header entries.hpp

template<Enum E, typename Pair = std::pair<E,std::string_view>,bool NullTerminated = true>
inline constexpr std::array<Pair,count<E>> entries;
```

- **Description**:  
  Gives an array containing all the string names of the enum and the values, it is sorted in ascending order.
  **Notes**: This variable is overridable if needed for compile time performance, all other enchantum varaibles synthesize from `entries<E>`.

  **Notes**: If you don't need the elements to have an address (you just want to iterate on them) use [`entries_generator`](#entries_generator) instead.

- **Parameters**:

  `E`: enum to generate value-string entries for.
  
  `Pair`: pair type used default to `std::pair<E,std::string_view>`

  `NullTerminated` determines whether the strings should point to null terminated character arrays `true` by default but can be disabled if not needed (to save static memory) via setting of this flag to `false`.

- **Example**:

```cpp
enum class Color { Red, Green = -2, Blue };
for (const auto& [value,string] : enchantum::entries<Color>) {
    std::cout << static_cast<int>(value) << " = " << name << std::endl;
}
// Outputs: 
// -2 = "Green"
// -1 = "Blue"
// 0 = "Red"
```

### `values`

```cpp
// defined in header entries.hpp

template<Enum E>
constexpr std::array<E,count<E>> values;
```

**Description**:  
  Gives an array containing all the values of the enum type equalivent to taking the elements of `entries<E>` in sorted order.

  **Notes**: If you don't need the elements to have an address (you just want to iterate on them) use [`values_generator`](#values_generator) instead.

> Example
```cpp
enum class Color { Red, Green, Blue };

for (auto value : enchantum::values<Color>)
    std::cout << static_cast<int>(value) << std::endl;
// Outputs: 0, 1, 2 (Red, Green, Blue)
```

---

### `names`

```cpp
// defined in header entries.hpp

template<Enum E, typename String = std::string_view,bool NullTerminated = true>
inline constexpr std::array<String,count<E>> names;
```

- **Description**:  
  Gives an array containing all the string names of the enum values equalivent to taking all the strings of `entries<E>`.

  **Notes**: If you don't need the elements to have an address (you just want to iterate on them) use [`names_generator`](#names_generator) instead.
  
- **Parameters**:

  `E`: enum to generate value-string entries for.
  
  `String`: string type used default to `std::string_view` it is required to be constexpr constructible.

  `NullTerminated` determines whether the strings should point to null terminated character arrays `true` by default but can be disabled if not needed (to save static memory) via setting of this flag to `false`.

**Example**:

```cpp
enum class Color { Red, Green, Blue };
for (auto name : enchantum::names<Color>)
    std::cout << name << std::endl;
// Outputs: "Red", "Green", "Blue"
```

### `values_generator`

```cpp
// defined in header generators.hpp

template<Enum E>
inline constexpr /*implementation details*/ values_generator;
```

**Description**:  
  Gives a generating iteratable object for enum values.
  This is different than `values`, since it does not store the values it creates them on the fly.
  In general if you don't need actual storage and addressability of the enum values but merely iterate on them use the `values_generator` variable instead it yeilds better binary sizes.

> Example
```cpp
enum class Color { Red, Green, Blue };

for (Color value : enchantum::values_generator<Color>)
    std::cout << static_cast<int>(value) << std::endl;
// Outputs: 0, 1, 2 (Red, Green, Blue)
```

### `names_generator`

```cpp
// defined in header generators.hpp

template<typename E,typename String = std::string_view,NullTerminated = true>
inline constexpr /*implementation details*/ names_generator;
```

**Description**:  
  Gives a generating iteratable object for enum names.
  This is different than `names`, since it does not store the names it creates them on the fly.
  In general if you don't need actual storage and addressability of the enum names but merely iterate on them use the `names_generator` variable instead it yeilds better binary sizes.


> Example
```cpp
enum class Color { Red, Green, Blue };

for (std::string_view s : enchantum::names_generator<Color>)
    std::cout << s << std::endl;
// Outputs: 
// Red 
// Green
// Blue
```

### `entries_generator`

```cpp
// defined in header generators.hpp

template<typename E,typename Pair = std::pair<E,std::string_view>,NullTerminated = true>
inline constexpr /*implementation details*/ entries_generator;
```

**Description**:  
  Gives a generating iteratable object for enum names.
  This is different than `entries`, since it does not store the entries it creates the them on the fly.
  In general if you don't need actual storage and addressability of the enum entries but merely iterate on them use the `entries_generator` variable instead it yeilds better binary sizes.

> Example
```cpp
enum class Color { Red, Green, Blue };

for (auto [value,name] : enchantum::entries_generator<Color>)
    std::cout << name << " = " << static_cast<int>(value) << std::endl;
// Outputs: 
// Red = 0
// Green = 1
// Blue = 2
```

---

### `contains`

```cpp
// defined in header enchantum.hpp

template<Enum E>
constexpr bool contains(E value) noexcept;

template<Enum E>
constexpr bool contains(std::underlying_type_t<E> value) noexcept;

template<Enum E>
constexpr bool contains(std::string_view name) noexcept;

template<Enum E, std::predicate<std::string_view, std::string_view> BinaryPredicate>
constexpr bool contains(std::string_view name, BinaryPredicate binary_predicate) noexcept;

```

- **Description**:  
  Checks if a specific enum value, underlying value, or string name is present in the enum.

**Notes**:
  Additional overloads may be provided to optimize for specific properties (e.g enums with no gaps can compare against `min<E>` and `max<E>`)

- **Parameters**:
  - `value`: The enum value or underlying integer value to check.
  - `name`: A string view representing the name of the enum to check.
  - `binary_predicate`: comparator for checking whether 2 `std::string_view`s are equal
- **Returns**:  
  `true` if the value or name is present in the enum, `false` otherwise.

- **Example**:
```cpp
enum class Color { Red, Green, Blue };

enchantum::contains(Color::Red);  // true
enchantum::contains<Color>(3);    // false, no such value
enchantum::contains<Color>("Green"); // true
enchantum::contains<Color>("GreEn"); // false case sensitive

```

### `contains_bitflag`

```cpp
// defined in header bitflags.hpp

template<BitFlagEnum E>
constexpr bool contains_bitflag(E value) noexcept;

template<BitFlagEnum E>
constexpr bool contains_bitflag(std::underlying_type_t<E> value) noexcept;

template<BitFlagEnum E>
constexpr bool contains_bitflag(std::string_view name,char sep = '|') noexcept;

template<BitFlagEnum E, std::predicate<std::string_view, std::string_view> BinaryPred>
constexpr bool contains_bitflag(std::string_view s, char sep, BinaryPred binary_pred) noexcept;

```

**Description**:

The `contains_bitflag` function checks whether a given value, underlying value, or string representation corresponds to a valid bitflag combination for the bitflag enum `E`.

1. Checks enum values, it checks if all set bits in the value are a valid bitwise `or` combination of `values<E>`

2. Same as `1` but uses underlying type.

3. For strings, it parses the string (like `cast_bitflag`)

4. same as `3` but with custom comparator.

It is typically used to verify that a bitmask is composed only of valid bit flags or that a string correctly represents a valid combination.

**Parameters**:
- `value`: The enum value or underlying type to check.
- `name`: The string containing flag names, separated by the `sep` character.
- `sep`: A character used to delimit individual flag names (default: '|').
- `binary_pred`: A predicate to compare the input string segments to the enum `names<E>` the first arguement of the predicate is always the input `name` and the second arguement is the segment being compared.

**Returns**:

1. `true` if the value contains only valid flags (i.e., if all set bits are valid bit flags).
`false` otherwise.

2. same as `1`

3. if string is a valid bitflag return `true` otherwise `false`

4. same as 4 but with custom comparator.

- **Example**:
```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/bitflags.hpp>

enum class Permissions : std::uint8_t {
  None = 0,
  Read = 1 << 0,
  Write = 1 << 1,
  Execute = 1 << 2
};

ENCHANTUM_DEFINE_BITWISE_FOR(Permissions)

Permissions value = Permissions::Read | Permissions::Write;

// Output: 
// true 
// false
std::cout << contains_bitflag(value) << '\n';
std::cout << contains_bitflag(value | Permissions(1<<3)) << '\n';

```

### Scoped Functions

There is scoped variants for functions `cast`,`to_string`,`contains`,`cast_bitflag`,`to_string_bitflag`,`contains_bitflag`.

They ocheck or output the scope.

```cpp
// defined in header scoped.hpp
namespace scoped {

namespace details {
  struct TO_STRING_FUNCTOR {
    template<Enum E>
    std::string_view operator()(E value) const noexcept;
  };

  template<Enum E>
  struct CAST_FUNCTOR {
    constexpr std::optional<E> operator()(std::string_view name) const noexcept;

    template<std::predicate<std::string_view, std::string_view> BinaryPred>
    constexpr std::optional<E> operator(std::string_view name, BinaryPred binary_predicate)() const noexcept;
  };
}

inline constexpr details::TO_STRING_FUNCTOR to_string;

template<Enum E>
inline constexpr details::CAST_FUNCTOR<E> cast;

template<Enum E>
constexpr bool contains(std::string_view name) noexcept;

template<Enum E, std::predicate<std::string_view, std::string_view> BinaryPredicate>
constexpr bool contains(std::string_view name, BinaryPredicate binary_predicate) noexcept;


template<BitFlagEnum E>
string to_string_bitflag(E value, char sep = '|');

template<BitFlagEnum E, std::predicate<std::string_view, std::string_view> BinaryPred>
constexpr std::optional<E> cast_bitflag(std::string_view s, char sep, BinaryPred binary_pred) noexcept;

template<BitFlagEnum E>
constexpr std::optional<E> cast_bitflag(std::string_view s, char sep = '|') noexcept;

template<BitFlagEnum E, std::predicate<std::string_view, std::string_view> BinaryPred>
constexpr bool contains_bitflag(std::string_view s, char sep, BinaryPred binary_pred) noexcept;

template<BitFlagEnum E>
constexpr bool contains_bitflag(std::string_view s, char sep = '|') noexcept;

} // namespace scoped
```

- **Example**:
```cpp
#include <enchantum/scoped.hpp>
#include <enchantum/bitwise_operators.hpp>
enum class InputModifiers { 
    None     = 0,
    Shift    = 1 << 0,
    Control  = 1 << 1,
    Alt      = 1 << 2,
};
ENCHANTUM_DEFINE_BITWISE_FOR(InputModifiers);

int main() {
  // Outputs: "InputModifiers::Shift"
  std::cout << enchantum::scoped::to_string(InputModifiers::Shift);

  // Outputs: "InputModifiers::Control|InputModifiers::Alt"
  std::cout << enchantum::scoped::to_string_bitflag(InputModifiers::Control|InputModifiers::Alt);

  std::optional<InputModifiers> cast = enchantum::scoped::cast("InputModifiers::Shift");
  // cast.value() == InputModifiers::Shift

  std::optional<InputModifiers> cast_bitflag = enchantum::scoped::cast_bitflags("InputModifiers::Control|InputModifiers::Alt");
  // cast.value() == InputModifiers::Control|InputModifiers::Alt (7)
}

```

### `index_to_enum`

```cpp
// defined in header enchantum.hpp

namespace details 
{
  template<Enum E>
  struct INDEX_TO_ENUM_FUNCTOR {
    constexpr std::optional<E> operator()(std::size_t i) const noexcept;
  };
}

template<Enum E>
constexpr inline details::INDEX_TO_ENUM_FUNCTOR<E> index_to_enum;
```

- **Description**:  
  Converts an index to its corresponding enum value. The index must be within the valid range of the enum values.

- **Parameters**:
  - `i`: The index to convert to an enum value.

- **Returns**:  
  The enum value corresponding to the provided index. if the index is out of bounds `std::nullopt` is returned.

- **Example**:
```cpp
#include <enchantum/enchantum.hpp>
enum class Color { Red, Green = 42, Blue };

std::optional<Color> color = enchantum::index_to_enum<Color>(1);
std::cout << static_cast<int>(*color) << std::endl;  // Outputs: 42 (Green)
```

### `enum_to_index`

```cpp
// defined in header enchantum.hpp

namespace details 
{
  struct ENUM_TO_INDEX_FUNCTOR {
    template<Enum E>
    constexpr std::optional<std::size_t> operator()(E value) const noexcept;
  };
}

constexpr inline details::ENUM_TO_INDEX_FUNCTOR enum_to_index;

```

- **Description**:  
  Converts an enum to its corresponding index value.

- **Parameters**:
  - `value`: The enum to convert to an index value.

- **Returns**:  
  The index value corresponding to the provided enum. if the enum is not a value in `values<E>` `std::nullopt` is returned.

- **Example**:
```cpp
#include <enchantum/enchantum.hpp>
enum class Color { Red, Green = 42, Blue };

std::optional<std::size_t> index = enchantum::enum_to_index(Color::Green);
std::cout << *index << std::endl;  // Outputs: 1 (Green)
```

---

### `next_value`/`prev_value`

```cpp
// defined in header next_value.hpp

namespace details {

  struct NEXT_VALUE_FUNCTOR {
    template<Enum E>
    constexpr std::optional<E> operator()(E value, std::ptrdiff_t n = 1) noexcept;
    };
    struct NEXT_VALUE_CIRCULAR_FUNCTOR {
       template<Enum E>
       constexpr E operator()(E value, std::ptrdiff_t n = 1) noexcept;
    };
}

inline constexpr details::NEXT_VALUE_FUNCTOR next_value;
inline constexpr details::PREV_VALUE_FUNCTOR prev_value;
inline constexpr details::NEXT_VALUE_CIRCULAR_FUNCTOR next_value_circular;
inline constexpr details::PREV_VALUE_CIRCULAR_FUNCTOR prev_value_circular;

```

- **Description**:
These function objects allow navigation through the enum values either in a linear or circular manner. You can move forward or backward through the enum values by providing an `n` value (steps). If `n` is positive, it moves forward; if `n` is negative, it moves backward.

  `next_value`: returns the next value in the enum sequence 

  `next_value_circular`: returns the next value in the enum sequence, wrapping around if the end is reached.

  `prev_value`: returns the previous value in the enum sequence equal to `next_value(value,-n)`.

  `prev_value_circular`: returns the previous value in the enum sequence, wrapping around if the start is reached equal to `next_value_circular(value,-n)`.

- **Notes**:
The circular variants require `value` to be a valid enum otherwise an assertion is called.

They are functors and not templated functions which allows passing  them to higher order functions.

- **Parameters**:
  - `value`: The enum to convert to an index value.

- **Returns**:  
`next_value` and `prev_value` return a std::optional<E> representing the next or previous enum value, or std::nullopt if the value is out of range or invalid.

`next_value_circular` and `prev_value_circular` return the next or previous enum value, respectively, with circular wrap-around behavior, ensuring the returned value stays within the valid range.

- **Example**:
```cpp
#include <enchantum/next_value.hpp>

enum class Direction { North, East, South, West };

std::optional<Direction> next = enchantum::next_value(Direction::East);  
std::cout << (next.has_value() ? "Found next value" : "No next value") << std::endl;  
// Outputs: Found next value

Direction nextCircular = enchantum::next_value_circular(Direction::West);  
std::cout << "Circular next value: " << static_cast<int>(nextCircular) << std::endl;  
// Outputs: Circular next value: 0 (North)

std::optional<Direction> prev = enchantum::prev_value(Direction::South);  
std::cout << (prev.has_value() ? "Found previous value" : "No previous value") << std::endl;  
// Outputs: Found previous value

Direction prevCircular = enchantum::prev_value_circular(Direction::North);  
std::cout << "Circular previous value: " << static_cast<int>(prevCircular) << std::endl;  
// Outputs: Circular previous value: 3 (West)

```

---

### `is_bitflag`

```cpp
template<Enum E>
inline constexpr bool is_bitflag =  
(
       requires(E e) { { e & e } -> std::same_as<E>; }
    || requires(E e) { { e & e } -> std::same_as<bool>; }
) &&
requires(E e) {
    { ~e } -> std::same_as<E>;
    { e | e } -> std::same_as<E>;
    { e &= e } -> std::same_as<E&>;
    { e |= e } -> std::same_as<E&>;
};
```

- **Description**:  
  Checks if an enum is a bitflag enum, i.e., an enum that supports bitwise operations such as `&`, `|`, and `~`.
  you can override this variable for specific enums if needed (e.g `operator&` returns a proxy comparable to bool and convertible to the enum value )
  or make it `false` to disallow treating as bitflag.
  
- **Returns**:  
  `true` if the enum supports bitwise operations, `false` otherwise.

- **Example**:
```cpp
#include <enchantum/common.hpp>

enum class Flags : uint32_t {
    None = 0,
    FlagA = 1 << 0,
    FlagB = 1 << 1,
    FlagC = 1 << 2
};
  
Flags operator~(Flags);
bool   operator&(Flags, Flags); // can return `Flags` as well
Flags  operator|(Flags, Flags);
Flags& operator|=(Flags&, Flags);
Flags& operator&=(Flags&, Flags);

static_assert(enchantum::is_bitflag<Flags>);  // true
```

### `is_contiguous`

```cpp
template<typename>
constexpr inline bool is_contiguous = false;

template<Enum E>
constexpr inline bool is_contiguous<E> = /*implementation details*/;

```

Checks whether an enum is contiguous. All members are sequential.

Defined in header `enchantum.hpp`

### `is_contiguous_bitflag`

```cpp
template<typename>
constexpr inline bool is_contiguous_bitflag = false;

template<BitFlagEnum E>
constexpr inline bool is_contiguous_bitflag<E> = /*implementation details*/;

```

Checks whether a bitflag enum is contiguous. All members are sequentially powers of 2 (excluding `0` value).

Defined in header `enchantum.hpp`

---

# Stream Operators

The `operator>>` and `operator<<` are provided in the `enchantum` library to enable **streaming** of enum values to and from input/output streams. These operators are defined in the nested `istream_operators` and `ostream_operators` namespaces in headers `istream.hpp` and `ostream.hpp` respectivly.

There is also the convienence header `iostream.hpp` which includes both of them and has a new nested namespace that contains the istream operators and ostream operators

They are not `[[nodiscard]]`.

## iostream support

```cpp
// defined in header iostream.hpp
namespace iostream_operators {
  template<typename Traits,Enum E>
  std::basic_ostream<char,Traits>& operator<<(std::basic_ostream<char,Traits>& os, E value);

  template<typename Traits,Enum E>
  requires std::assignable_from<E&,E>
  std::basic_istream<char,Traits>& operator>>(std::basic_istream<char,Traits>& os, E& value);
}
```

These functions are not `[[nodiscard]]`.

- **Example**:
```cpp
#include <enchantum/iostream.hpp>

enum class Animals { Monkey,Giraffe,Elephant}

int main() {
  using enchantum::iostream_operators::operator<<;
  // Outputs: Monkey
  std::cout << Animals::Monkey;

  using enchantum::iostream_operators::operator>>;
  Animals animal;
  // If input is "Giraffe" then `animal` is `Animals::Giraffe`
  std::cin >> animal;
}


```

## `fmt::format` / `std::format` support

There is headers for them. that provide `std::formatter`/`fmt::formatter` for all enums.

`fmt_format.hpp`/`std_format.hpp`

```cpp
#include <format> // or <fmt/format.h>
#include <enchantum/std_format.hpp> // or fmt_format.hpp

enum class Letters {a,b,c,e,d};
std::cout << std::format("{} then {} then {}",Letters::a,Letters::b,Letters::c); // a then b then c
```

### for_each

```cpp
template <Enum E, typename Function>
constexpr void for_each(Function function) /*noexcept when possible*/;
```

**Example**:

```cpp
#include <enchantum/algorithms.hpp>
enum class Numbers {
  _0,_1,_2,_3,_4,_5
}
std::underlying_type_t<Numbers> sum{};
enchantum::for_each<Color>([&sum](auto constant) {
  constexpr std::underlying_type_t<Numbers> v = enchantum::to_underlying(constant.value); // constant is `std::integral_constant`
  sum += v;
});
```

### to_underlying

Same as [std::to_underlying](https://en.cppreference.com/w/cpp/utility/to_underlying) just ported to backward versions.

```cpp
template <Enum E>
constexpr std::underlying_type_t<E> to_underlying(E e) noexcept;
```

Defined in header `entries.hpp` which is included everywhere.

### array

```cpp
// defined in header `array.hpp`
template <Enum E, typename T>
class array : public std::array<T,count<E>> {
private:
  using base = std::array<T,count<E>>;
public:
  using index_type = E;
  using typename base::reference;
  using typename base::const_reference;

  using base::at;
  using base::operator[];

  constexpr reference at(E pos);
  constexpr const_reference at(E pos) const;
  constexpr reference operator[](E pos) noexcept;
  constexpr const_reference operator[](E pos) const noexcept;
}
```

**Example**
```cpp
#include <enchantum/array.hpp>
#include <enchantum/enchantum.hpp>

enum class Color { Red, Green, Blue };
enchantum::array<Color, std::uint32_t> values = {
    0xff'00'00'00,  // Red
    0x00'ff'00'00,  // Green
    0x00'00'ff'00   // Blue
  };

std::cout << values[Color::Green] << '\n';  // Outputs: 16711680
values[Color::Blue] = 42;
std::cout << values.at(Color::Blue) << '\n';  // Outputs: 42
```


### bitset

```cpp
// defined in header `bitset.hpp`
template<Enum E>
class bitset : public std::bitset<count<E>> {
private:
  using base = std::bitset<count<E>>;
public:
  using typename base::reference;

  using base::operator[];
  using base::flip;
  using base::reset;
  using base::set;
  using base::test;

  using base::base;
  using base::operator=;

  [[nodiscard]] constexpr std::string to_string(char sep = '|') const;

  [[nodiscard]] constexpr auto to_string(char zero,char one) const;

  constexpr bitset(std::initializer_list<E> values) noexcept;

  [[nodiscard]] constexpr reference operator[](E pos) noexcept;
  [[nodiscard]] constexpr bool operator[](E pos) const noexcept;
  constexpr bool test(E pos);
  constexpr bitset& set(E pos, bool value = true);
  constexpr bitset& reset(E pos);
  constexpr bitset& flip(E pos);
};

template<typename E>
struct std::hash<enchantum::bitset<E>> : std::hash<std::bitset<enchantum::count<E>>> {
  using std::hash<std::bitset<enchantum::count<E>>>::operator();
};
```

**Example**
```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitset.hpp>

enum class Color { Red, Green, Blue };
enchantum::bitset<Color> bitset{Color::Red,Color::Green}

std::cout << bitset[Color::Green] << '\n';  // Outputs: 1 
bitset[Color::Green] = false;
std::cout << bitset.test(Color::Green) << '\n';  // Outputs: 0
std::cout << bitset.count() << '\n';  // Outputs: 2 (2 bits are set)

```

### ENCHANTUM_DEFINE_BITWISE_FOR

- **Description**: 
Overloads the bitwise operators for a given enum. `~`,`&`,`|`,`^`,`&=`,`|=`,`^=`

**Notes**: the macro does not require a semicolon at the end.
```cpp
// defined in header `bitwise_operators.hpp`
#define ENCHANTUM_DEFINE_BITWISE_FOR(Enum)                                                \
  constexpr Enum operator&(Enum a, Enum b) noexcept                                       \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) & static_cast<T>(b));                      \
  }                                                                                       \
  constexpr Enum operator|(Enum a, Enum b) noexcept                                       \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) | static_cast<T>(b));                      \
  }                                                                                       \
  constexpr Enum operator^(Enum a, Enum b) noexcept                                       \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) ^ static_cast<T>(b));                      \
  }                                                                                       \
  constexpr Enum&              operator&=(Enum& a, Enum b) noexcept { return a = a & b; } \
  constexpr Enum&              operator|=(Enum& a, Enum b) noexcept { return a = a | b; } \
  constexpr Enum&              operator^=(Enum& a, Enum b) noexcept { return a = a ^ b; } \
  constexpr Enum operator~(Enum a) noexcept                                               \
  {                                                                                       \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(a));              \
  }
```


### ENCHANTUM_ASSERT

- **Description**: 
A customizable macro for assertions in the library.

The `__VA_ARGS__` at the end is more info in the macro for example local variables are put there so if you have a macro that can display more stuff you can override this macro by defining `#define ENCHANTUM_ASSERT(cond,msg,...) MY_COOL_ASSERT(cond,msg,__VA_ARGS__)` before including `common.hpp`

```cpp
// defined in header `common.hpp`
#ifndef ENCHANTUM_ASSERT
#define ENCHANTUM_ASSERT(cond,msg,...) assert(cond && msg)
#endif
```


### ENCHANTUM_THROW

- **Description**: 
A customizable macro for throwing exception in the library.

The `__VA_ARGS__` at the end is more info in the macro for example local variables are put there so if you have a way to output more info within an exception, can override this macro by defining `#define ENCHANTUM_THROW(exception,...) MY_THROW_MACRO(exception,__VA_ARGS__)` before including `common.hpp`

```cpp
// defined in header `common.hpp`
#ifndef ENCHANTUM_THROW
#define ENCHANTUM_THROW(exception,...) throw exception
#endif
```


### ENCHANTUM_ENABLE_MSVC_SPEEDUP

- **Description**: 
A boolean macro that speeds up msvc compile times but may cause issues with extremely large enums ranges. it is on by default and can be overriden.

```cpp
// defined in header `entries.hpp`
#ifndef ENCHANTUM_ENABLE_MSVC_SPEEDUP
#define ENCHANTUM_ENABLE_MSVC_SPEEDUP 1
#endif
```

### ENCHANTUM_OPTIONAL

- **Description**: 
A macro for customizing the optional type used in the library it is by default `std::optional`
```cpp
// in all headers
#ifndef ENCHANTUM_OPTIONAL
#include <optional>
#define ENCHANTUM_OPTIONAL using std::optional;
#endif
```

### ENCHANTUM_STRING

- **Description**: 
A macro for customizing the string type used in the library it is by default `std::string`
```cpp
// in all headers
#ifndef ENCHANTUM_STRING
#include <string>
#define ENCHANTUM_STRING using std::string;
#endif
```

### ENCHANTUM_STRING_VIEW

- **Description**: 
A macro for customizing the string view type used in the library it is by default `std::string_view`
```cpp
// in all headers
#ifndef ENCHANTUM_STRING_VIEW
#include <string_view>
#define ENCHANTUM_STRING_VIEW using std::string_view;
#endif
```


### ENCHANTUM_CONFIG_FILE

- **Description**: 
A macro for customizing a file that will be included in all enchantum files

```cpp
// in all headers
#ifdef ENCHANTUM_CONFIG_FILE
#include ENCHANTUM_CONFIG_FILE
#endif
```

> Example:

```cpp
#define ENCHANTUM_CONFIG_FILE "my_enchantum_config.hpp"
#include <enchantum/enchantum.hpp>
```

where `my_enchantum_config.hpp` is 

```cpp
#include "my_optional.hpp"
#define  ENCHANTUM_OPTIONAL template<typename T> using optional = my_optional<T>;
#include "my_string.hpp"
#define  ENCHANTUM_STRING using string = my_string;
```