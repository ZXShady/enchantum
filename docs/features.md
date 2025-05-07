All the functions/variables are defined in namespace `enchantum`

**Note**: Documentation Incomplete.

Quick Reference

**Concepts And Traits**:
- [Enum](#enum)
- [SignedEnum](#signedenum)
- [UnsignedEnum](#unsignedenum)
- [ScopedEnum](#scopedenum)
- [UnscopedEnum](#unscopedenum)
- [BitFlagEnum](#bitflagenum)
- [ContiguousEnum](#contiguousenum)
- [EnumOfUnderlying](#enumofunderlying)
- [has_zero_flag](#has_zero_flag)
- [enum_traits](#enum_traits)

**Functions**:
- [to_string](#to_string)
- [to_string_bitflag](#to_string_bitflag)
- [cast](#cast)
- [cast_bitflag](#cast_bitflag)
- [contains](#contains)
- [contains_bitflag](#contains_bitflag)
- [index_to_enum](#index_to_enum)
- [enum_to_index](#enum_to_index)
- [std::format/fmt::format](#fmtformat--stdformat-support)
- [operator<<](#operator-stream-output-operator)
- [operator>>](#operator-stream-input-operator)

**Constants**:
  - [entries](#entries)
  - [names](#names)
  - [values](#values)
  - [max](#max)
  - [min](#min)
  - [count](#count)

**Containers**:
  - [array](#array)

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
The BitFlagEnum concept is used for enums that support bitwise operations, such as &, |, and ~. This concept can be used to check if an enum is intended for bitflag operations, where each enum value represents a distinct bit.

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
    FlagC = 1 << 2
};

Flags operator&(Flags a,Flags b); // can return bool
Flags operator|(Flags a,Flags b);
Flags operator~(Flags x);
Flags& operator&=(Flags a,Flags b);
Flags& operator|=(Flags a,Flags b);


static_assert(enchantum::BitFlagEnum<Flags>);
```

## ContiguousEnum

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

### EnumOfUnderlying

```cpp
// defined in header `common.hpp`

template<typename E, typename Underlying>
concept EnumOfUnderlying = Enum<E> && std::same_as<std::underlying_type_t<E>, Underlying>;
```

> Example usage:
```cpp
#include <enchantum/common.hpp>

enum class Status : char { Ok = 0, Error = 1, Unknown = 2 };
static_assert(enchantum::EnumOfUnderlying<Status,char>);
```

### has_zero_flag

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

### enum_traits

```cpp
// defined in header `common.hpp`

template<typename T>
struct enum_traits;

template<Enum E>
struct enum_traits // default
{
  constexpr static auto prefix_length = 0;
  constexpr static auto min = ENCHANTUM_MIN_RANGE;
  constexpr static auto max = ENCHANTUM_MAX_RANGE;
};
```
**Description**:
  A customization point for `enum`s by this library like setting `prefix_length`,`min`,`max` instead of using default values.

> Example usage:
```cpp
#include <enchantum/enchantum.hpp>
#include <enchantum/bitwise_operators.hpp>
// this is bug enum it is outside of default range [-256,256] and it has this annoying prefix
enum BigEnumOutsideOfDefault : std::uint16_t {
  BigEnumOutsideOfDefault_A =0,BigEnumOutsideOfDefault_B = 4096 
};

template<>
struct enchantum::enum_traits<BigEnumOutsideOfDefault> {
  constexpr static auto prefix_length = sizeof("BigEnumOutsideOfDefault_")-1;
  constexpr static auto min = 0;
  constexpr static auto max = 4096;
};


// prefix removed
static_assert(enchantum::names<BigEnumOutsideOfDefault>[0] == "A");
static_assert(enchantum::names<BigEnumOutsideOfDefault>[1] == "B");
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
  and the "function" is a functor allowing easy passing of higher order functions without manually specifying the enum type could be handy in `ranges`.

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
[[nodiscard]] constexpr String to_string_bitflag(E value, char sep = '|');

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
template<Enum E>
constexpr std::optional<E> cast(std::underlying_type_t<E> e) noexcept;

template<Enum E>
constexpr std::optional<E> cast(std::string_view name) noexcept;


template<Enum E, std::predicate<std::string_view, std::string_view> BinaryPred>
constexpr optional<E> cast(std::string_view name, BinaryPred binary_predicate) noexcept

```
**Description**:

 1. Attempts to convert an integral value to the corresponding enum value if it is a valid underlying value (i.e it is one of the elements of `values<E>`).
 2. Attempts to convert a `std::string_view` to the corresponding enum value based on its name it is case sensitive.
 3. Attempts to convert a `std::string_view` to the corresponding enum value based using a predicate. **note** the first arguement of the predicate is the `cast` `name` arguement while the second arguement of the predicate is the names of the enum of `names<E>`.

Returns:
1. An `optional<E>` containing the enum value if the integer matches a defined enum value; otherwise, `std::nullopt`.
2. An `optional<E>` containing the enum value if the `name` matches a defined enum name; otherwise, `std::nullopt`.
3. Same as 2.

**Examples**:
```cpp
#include <enchantum/cast.hpp>

enum class Status { Ok = 0, Error = 1, Unknown = 2 };
assert(enchantum::cast<Status>(1).has_value());
assert(!enchantum::cast<Status>(300).has_value());

assert(enchantum::cast<Color>("Unknown").has_value());

assert(!enchantum::cast<Color>("UnKnoWn").has_value());

assert(enchantum::cast<Color>("UnKnOwn",[](std::string_view a,std::string_view b){
  return std::ranges::equal(a,b,[](unsigned char x,unsigned char y){
    return std::tolower(x) == std::tolower(y);
  })
}).has_value());

```

### `cast_bitflag`

```cpp
// defined in header bitflags.hpp

template<BitFlagEnum E, std::predicate<std::string_view, std::string_view> BinaryPred>
[[nodiscard]] constexpr std::optional<E> cast_bitflag(std::string_view name, char sep, BinaryPred binary_pred) noexcept;

template<BitFlagEnum E>
[[nodiscard]] constexpr std::optional<E> cast_bitflag(std::string_view name, char sep = '|') noexcept;

template<BitFlagEnum E>
[[nodiscard]] constexpr std::optional<E> cast_bitflag(E value) noexcept;
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
ENCHANTUM_DEFINE_BITWISE_FOR(Flags)

std::optional<Permissions> p = enchantum::cast_bitflag<Permissions>("Write|Read");
assert(p.has_value() && (*p == (Permissions::Read | Permissions::Write)));

// Case-insensitive parsing
std::optional<Permissions> ci = enchantum::cast_bitflag<Permissions>("read|WRITE", '|',
  [](std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](unsigned char x, unsigned char y) {
      return std::tolower(x) == std::tolower(y);
    });
  }
);
assert(ci.has_value() && (*ci == (Permissions::Read | Permissions::Write)));

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

- **Parameters**:

  `E`: enum to generate value-string entries for.
  
  `Pair`: pair type used default to `std::pair`

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

> Example
```cpp
enum class Color { Red, Green, Blue };

for (auto value : enchantum::values<Color>) {
    std::cout << static_cast<int>(value) << std::endl;
}
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

- **Parameters**:

  `E`: enum to generate value-string entries for.
  
  `Pair`: pair type used default to `std::pair`

  `NullTerminated` determines whether the strings should point to null terminated character arrays `true` by default but can be disabled if not needed (to save static memory) via setting of this flag to `false`.

- **Example**:

```cpp
enum class Color { Red, Green, Blue };
for (auto name : enchantum::names<Color>) {
    std::cout << name << std::endl;
}
// Outputs: "Red", "Green", "Blue"
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

```

- **Description**:  
  Checks if a specific enum value, underlying value, or string name is present in the enum.

**Notes**:
  Additional overloads may be provided to optimize for specific properties (e.g enums with no gaps can compare against `min<E>` and `max<E>`)

- **Parameters**:
  - `value`: The enum value or underlying integer value to check.
  - `name`: A string view representing the name of the enum to check.

- **Returns**:  
  `true` if the value or name is present in the enum, `false` otherwise.

- **Example**:
```cpp
enum class Color { Red, Green, Blue };

bool containsRed = enchantum::contains(Color::Red);  // true
bool containsBlue = enchantum::contains<Color>(3);  // false, no such value
bool containsGreenName = enchantum::contains<Color>("Green");  // true
```

### `contains_bitflag`

```cpp
// defined in header bitflags.hpp

template<BitFlagEnum E>
constexpr bool contains_bitflag(E value) noexcept;
```

**Description**:

The `contains_bitflag` function checks whether all bits set in a given `BitFlagEnum` value are valid flags in the enumeration. It is typically used to verify that a bitmask is composed only of valid bit flags.

**Parameters**:

value: The value to check. It should be a valid instance of the BitFlagEnum type, representing a bitmask where each bit corresponds to a specific flag.

**Returns**:

true if the value contains only valid flags (i.e., if all set bits are valid bit flags).
false otherwise.

Template Parameters:

    E: The BitFlagEnum type being checked.
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

Permissions value = static_cast<Permissions>(Permissions::Read | Permissions::Write);

// Check if the value contains valid flags
bool is_valid = contains_bitflag(value);
// Output: true
std::cout << "Permissions value is valid: " << std::boolalpha << is_valid << std::endl;
```

---

### `index_to_enum`

```cpp
// defined in header enchantum.hpp

template<Enum E>
constexpr std::optional<E> index_to_enum(std::size_t i) noexcept;
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

template<Enum E>
constexpr std::optional<std::size_t> enum_to_index(E e) noexcept;
```

- **Description**:  
  Converts an enum to its corresponding index value.

- **Parameters**:
  - `e`: The enum to convert to an index value.

- **Returns**:  
  The index value corresponding to the provided enum. if the enum is not a value in `values<E>` `std::nullopt` is returned.

- **Example**:
```cpp
#include <enchantum/enchantum.hpp>
enum class Color { Red, Green = 42, Blue };

std::optional<std::size_t> index = enchantum::enum_to_index(Color::Red);
std::cout << *index << std::endl;  // Outputs: 1 (Green)
```

---

### `is_bitflag`

```cpp
template<Enum E>
inline constexpr bool is_bitflag =  
(
    requires(E e) { { e & e } -> std::same_as<E>; } ||
    requires(E e) { { e & e } -> std::same_as<bool>; }
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

---

# Stream Operators

The `operator<<` and `operator>>` are provided in the `enchantum` library to enable **streaming** of enum values to and from input/output streams. These operators are defined in the nested `istream_operators` and `ostream_operators` namespaces in headers `istream.hpp` and `ostream.hpp` respectivly.

There is also the convienence header `iostream.hpp` which includes both of them and has a new nested namespace that contains the istream operators and ostream operators

## `operator<<` (Stream Output Operator)

```cpp
namespace ostream_operators {
  template<typename CharType,Enum E>
  std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, E value);
}
```


## `operator>>` (Stream Input Operator)

```cpp
namespace istream_operators {
  template<typename CharType,Enum E>
  std::basic_istream<CharType>& operator>>(std::basic_istream<CharType>& os, E& value);
}
```

**iostream.hpp**
```cpp
#include "istream.hpp"
#include "ostream.hpp"

namespace enchantum::iostream_operators {
using ::enchantum::istream_operators::operator>>;
using ::enchantum::ostream_operators::operator<<;
} // namespace enchantum::iostream_operators
```

## `fmt::format` / `std::format` support

There is headers for them. that provide `std::formatter`/`fmt::formatter` for all enums.

`fmt_format.hpp`/`std_format.hpp`

```cpp
#include <format> // or fmt
#include <enchantum/std_format.hpp> // or fmt

enum class Letters {a,b,c,e,d};
std::cout << std::format("{} then {} then {}",Letters::a,Letters::b,Letters::c); // a then b then c
```

### array

```cpp
// defined in header `array.hpp`
template <Enum E, typename V>
class array : public std::array<V,count<E>> {
public:
  using index_type = E;
  using typename base::const_iterator;
  using typename base::const_pointer;
  using typename base::const_reference;
  using typename base::const_reverse_iterator;
  using typename base::difference_type;
  using typename base::iterator;
  using typename base::pointer;
  using typename base::reference;
  using typename base::reverse_iterator;
  using typename base::size_type;
  using typename base::value_type;
  using std::array<V,count<E>>::at;
  using std::array<V,count<E>>::operator[];

  constexpr reference at(E pos);
  constexpr const_reference at(E pos) const;
  constexpr reference operator[](E pos) noexcept;
  constexpr const_reference operator[](E pos) const noexcept;
}
```

> **Examples**
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