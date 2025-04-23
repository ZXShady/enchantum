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
## Function

#### `to_string`

```cpp
template<Enum E>
constexpr std::string_view to_string(E value) noexcept
```

**Description**:
  Converts an enum value to its corresponding string representation. 
  **Note** the `std::string_view` points to null-terminated character array/ 

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

---

### `min`

```cpp
template<Enum E>
constexpr E min;
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
constexpr E max;
```

**Description**:  
  Gives the maximum enum value

**Example**:
  ```cpp
  enum class Status { Ok = -1, Error = 1, Unknown = 2 };

  auto maxValue = enchantum::max<Status>; // Status::Unknown
  std::cout << static_cast<int>(maxValue) << std::endl;  // Outputs: 2
  ```

---

### `count`

```cpp
template<Enum E>
constexpr std::size_t count;
```

**Description**:  
  Gives the number of enum values in the enum type.

**Example**:
  ```cpp
  enum class Status { Ok = -1, Error = 1, Unknown = 2 };
  std::cout << enchantum::count<Status> << std::endl;  // Outputs: 3
  ```

---

### `values`

```cpp
template<Enum E>
constexpr std::array<E,/*count*/> values;

**Description**:  
  Gives an array containing all the values of the enum type equalivent to taking the elements of `entries<E>` in sorted order.

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
template<Enum E, typename String = std::string_view>
inline constexpr std::array<String,/*count*/> names;
```

- **Description**:  
  Gives an array containing all the string names of the enum values equalivent to taking all the strings of `entries<E>`.

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
template<Enum E>
constexpr bool contains(E value) noexcept;

template<Enum E>
constexpr bool contains(std::underlying_type_t<E> value) noexcept;

template<Enum E>
constexpr bool contains(std::string_view name) noexcept;

```

- **Description**:  
  Checks if a specific enum value, underlying value, or string name is present in the enum.

- **Parameters**:
  - `value`: The enum value or underlying integer value to check.
  - `name`: A string view representing the name of the enum to check.

- **Returns**:  
  `true` if the value or name is present in the enum, `false` otherwise.

- **Example**:
  ```cpp
  enum class Color { Red, Green, Blue };

  bool containsRed = enchantum::contains(Color::Red);  // true
  bool containsBlue = enchantum::contains<Color::Red>(3);  // false, no such value
  bool containsGreenName = enchantum::contains<Color>("Green");  // true
  ```

---

#### `index_to_enum`

```cpp
template<Enum E>
constexpr E index_to_enum(std::size_t i) noexcept;
```

- **Description**:  
  Converts an index to its corresponding enum value. The index must be within the valid range of the enum values.

- **Parameters**:
  - `i`: The index to convert to an enum value.

- **Returns**:  
  The enum value corresponding to the provided index.

- **Example**:
  ```cpp
  enum class Color { Red, Green, Blue };

  auto color = enchantum::index_to_enum<Color>(1);
  std::cout << static_cast<int>(color) << std::endl;  // Outputs: 1 (Green)
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

- **Returns**:  
  `true` if the enum supports bitwise operations, `false` otherwise.

- **Example**:
  ```cpp
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

  bool isFlagEnum = enchantum::is_bitflag<Flags>;  // true
  ```

---

### for_each

// not documentaed
