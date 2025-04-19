All the functions/variables are defined in namespace `enchantum`

# Concepts
## Enum
The Enum concept is used to identify types that are valid C++ enums. Any type that is an enum, whether scoped or unscoped, signed or unsigned, will satisfy this concept.

```cpp
template<typename T>
concept Enum = std::is_enum_v<T>;
```

> Example usage:
```cpp
enum class Color { Red, Green, Blue };
static_assert(enchantum::Enum<Color>);
```

## SignedEnum

The SignedEnum concept restricts types to enums whose underlying type is a signed integral type. It ensures that only enums with a signed integral base type (e.g., int, short) are used.

```cpp
template<typename T>
concept SignedEnum = Enum<T> && std::signed_integral<std::underlying_type_t<T>>;
```

> Example usage:

```cpp
enum class SignedColor : int { Red = -1, Green, Blue };
static_assert(enchantum::SignedEnum<SignedColor>);
```

## UnsignedEnum

The UnsignedEnum concept restricts types to enums whose underlying type is an unsigned integral type. It ensures that only enums with an unsigned integral base type (e.g., unsigned int, unsigned char) are used.

**Note** `bool` is considered an `unsigned` type
```cpp
template<typename T>
concept UnsignedEnum = Enum<T> && !SignedEnum<T>;
```

> Example usage:

```cpp
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
enum UnscopedColor { Red, Green, Blue };
static_assert(enchantum::UnscopedEnum<UnscopedColor>);
```

## BitFlagEnum
The BitFlagEnum concept is used for enums that support bitwise operations, such as &, |, and ~. This concept can be used to check if an enum is intended for bitflag operations, where each enum value represents a distinct bit.

```cpp
template<typename T>
concept BitFlagEnum = Enum<T> && is_bitflag<T>;
```

> Example usage:
```cpp
enum class Flags : uint32_t {
    None = 0,
    FlagA = 1 << 0,
    FlagB = 1 << 1,
    FlagC = 1 << 2
};

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
    enum class Status { Ok = 0, Error = 1, Unknown = 2 };
    static_assert(enchantum::ContiguousEnum<Status>);
```