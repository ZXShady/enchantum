#pragma once
#include "common.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <string_view>
#include <type_traits>
#include <utility>
//#include <bit> // for bit_cast
namespace enchantum {

namespace details {

  // string view with the simplest code that does not consume any constexpr steps
  struct str_view {};

  template<typename>
  constexpr auto type_name_func() noexcept
  {
    constexpr auto funcname = std::string_view(
      __FUNCSIG__ + (sizeof("auto __cdecl enchantum::details::type_name_func<enum ") - 1));
    // (sizeof("auto __cdecl enchantum::details::type_name_func<") - 1)
    constexpr auto         size = funcname.size() - (sizeof(">(void) noexcept") - 1);
    std::array<char, size> ret;
    auto* const            ret_data      = ret.data();
    const auto* const      funcname_data = funcname.data();
    for (std::size_t i = 0; i < size; ++i)
      ret_data[i] = funcname_data[i];
    return ret;
  }

  //  constexpr bool has_single_valid_enum(const std::size_t      type_name_length,
  //                                       const std::string_view funcsig,
  //                                       std::int64_t           min,
  //                                       std::int64_t           max)
  //  {
  //#define SZC(str) (sizeof(str) - 1)
  //    // IMPORTANT THIS NAME SHOULD MATCH THE FUNCTION BELWO
  //    auto p = SZC("auto __cdecl enchantum::details::var_name_func<class std::array<enum ") + type_name_length +
  //      SZC(",") + numLength(max - min) + SZC(">{enum ") + type_name_length +
  //      ((SZC("(") + SZC("enum ") + type_name_length + SZC(")0x,")) * (max - min)) +
  //      (totalHexDigitsInRange(min, max - 1)) + SZC("}>(void) noexcept") - 1;
  //#undef SZC
  //    return funcsig.size() != p;
  //  }

  template<typename T>
  inline constexpr auto type_name = type_name_func<T>();

  constexpr int numLength(std::uint64_t number)
  {
    int count = 0;
    do {
      count += 1;
      number /= 10;
    } while (number != 0);
    return count;
  }

  constexpr int hexCharCount(std::uint64_t number)
  {
    int count = 1;
    while (number >>= 4) { // shift right by 4 bits (1 hex digit)
      ++count;
    }
    return count;
  }
  constexpr int totalHexDigitsInRange(int64_t start, int64_t end)
  {
    int total = 0;
    for (int64_t i = start; i <= end; ++i) {
      total += hexCharCount(static_cast<uint64_t>(i)); // convert negative to unsigned 2's comp
    }
    return total;
  }

  // explicitly specifiying the calling convention so it is consistent in the __FUNCSIG__
  template<auto V>
  constexpr auto __cdecl var_name_func() noexcept
  {
    constexpr auto type_name_length = type_name<typename decltype(V)::value_type>.size();
    constexpr auto min              = std::int64_t(V.front());
    constexpr auto max              = std::int64_t(V.back());

    // This monster is a way to discard useless long signatures it checks the longest possible
    // signature with all the values being casts because it is most likely and if it is then it discards it
#define SZC(x) (sizeof(x) - 1)
    //constexpr auto Asize      = V.size();
    //constexpr auto numLen     = numLength(Asize);
    //constexpr auto hex        = totalHexDigitsInRange(min, max);
    constexpr auto prefix_len = SZC("auto __cdecl enchantum::details::var_name_func<class std::array<enum ") +
      type_name_length + SZC(",") + numLength(V.size()) + SZC(">{enum ") + type_name_length;

    constexpr auto             siglen = (prefix_len + ((SZC("(enum ") + type_name_length + SZC(")0x,")) * V.size()) +
                             (totalHexDigitsInRange(min, max)) + SZC("}>(void) noexcept") - 1);
    constexpr std::string_view p      = __FUNCSIG__;
    constexpr auto             len    = SZC(__FUNCSIG__);
    if constexpr (len != siglen) {
      return __FUNCSIG__ + prefix_len;
    }
    else {
      return "";
    }
#undef SZC
    //constexpr auto funcname = std::string_view(
    //  __FUNCSIG__ + (sizeof("auto __cdecl enchantum::details::var_name_func<") - 1));
    //// (sizeof("auto __cdecl enchantum::details::type_name_func<") - 1)
    //constexpr auto         size = funcname.size() - (sizeof(">(void) noexcept") - 1);
    //std::array<char, size> ret;
    //auto* const            ret_data      = ret.data();
    //const auto* const      funcname_data = funcname.data();
    //for (std::size_t i = 0; i < size; ++i)
    //  ret_data[i] = funcname_data[i];
    //return ret;
  }


  template<auto V>
  inline constexpr std::string_view var_name = var_name_func<V>();

  constexpr std::size_t search_concat(const std::string_view a, const std::string_view b)
  {
    //constexpr std::string_view colon = "::";
    const std::size_t len_a = a.size();
    const std::size_t len_b = b.size();
    if (len_b + 2 > a.size())
      return a.npos;

    for (std::size_t i = 0; i <= len_a - len_b - 1; ++i)
      if (a.substr(i, len_b) == b)
        return i;
    //const auto next = a.substr(i + len_b, 2);
    // Check for "::"
    //if (next[0] == ')' || next[0] == ':')
    return a.npos;
  }

  template<std::array V>
  constexpr auto reflection_data() noexcept
  {
    constexpr auto funcsig   = std::string_view(details::var_name<V>);
    auto           funcsig_2 = std::string_view(details::var_name<V>);

    constexpr auto funcsig_len     = funcsig.size();
    constexpr auto reflection_data = [name = funcsig]() mutable {
      constexpr const auto& enum_type_name_storage = type_name<typename decltype(V)::value_type>;
      constexpr auto enum_type_name = std::string_view(enum_type_name_storage.data(), enum_type_name_storage.size());
      name.remove_suffix(sizeof(">(void) noexcept") - 1);
      struct {
        char                       buffer[funcsig_len + funcsig_len / 2]{};
        std::array<bool, V.size()> enum_is_valid{};
        std::size_t                valid_count{};
      } ret_val;
      std::size_t buffer_index        = 0;
      std::size_t enum_is_valid_index = 0;

      auto pos = search_concat(name, enum_type_name);
      while (pos != name.npos) {
        if (name[pos + enum_type_name.size()] == ':') { // if it is `EnumName::`
          pos += enum_type_name.size() + 2;             // 2 is :: len
          if (const auto p = name.find(",", pos); p != name.npos) {
            const auto enum_name = name.substr(pos, p - pos);
            for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
              ret_val.buffer[buffer_index + i] = enum_name[i];

            buffer_index += enum_name.size();
            buffer_index += 1;
            name.remove_prefix(p + 1); // one past comma
          }
          else {
            // Text is
            // A::Val} <- need to remove this bracket
            auto enum_name = name.substr(pos);
            enum_name.remove_suffix(1);
            for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
              ret_val.buffer[buffer_index + i] = enum_name[i];

            buffer_index += enum_name.size();
            name = std::string_view{}; // end of string comma
          }
          ++ret_val.valid_count;
          ret_val.enum_is_valid[enum_is_valid_index] = true;
        }
        else {
          name.remove_prefix(pos + enum_type_name.size() + 1); // skip pos and the typename and the ')'
        }
        if (enum_is_valid_index >= ret_val.enum_is_valid.size())
          return ret_val;
        ++enum_is_valid_index;


        pos = search_concat(name, enum_type_name);
      }
      return ret_val;
    }();

    return [reflection_data]() {
      constexpr auto min_length = [reflection_data]() {
        std::size_t len = 0;
        for (std::size_t i = 0; i < sizeof(reflection_data.buffer); ++i) {
          if (reflection_data.buffer[i] == '\0' && reflection_data.buffer[i + 1] == '\0')
            return len + 1;
          ++len;
        }
        return len;
      }();
      struct {
        char                       strings[min_length];
        std::array<bool, V.size()> is_valid;
        std::size_t                valid_count;
      } data;
      for (std::size_t i = 0; i < min_length; ++i) {
        data.strings[i] = reflection_data.buffer[i];
      }
      data.valid_count = reflection_data.valid_count;
      data.is_valid    = reflection_data.enum_is_valid;
      return data;
    }();
  }

  template<auto V>
  inline constexpr auto static_var = reflection_data<V>();

  template<typename Enum, auto Min, auto Max>
  constexpr auto array_of_enums()
  {
    if constexpr (BitFlagEnum<Enum>) {
      using T                                    = std::underlying_type_t<Enum>;
      constexpr std::size_t                 bits = sizeof(T) * CHAR_BIT;
      std::array<std::array<Enum, bits>, 1> a{};
      for (std::size_t i = 0; i < bits; ++i)
        a[0][i] = static_cast<Enum>(static_cast<std::make_unsigned_t<T>>(1) << i);
      return a;
    }
    else {
      static_assert(Min < Max, "Min must be less than Max");

      constexpr auto        ChunkSize = ENCHANTUM_SEARCH_PER_ITERATION;
      constexpr std::size_t total     = Max - Min;
      constexpr std::size_t chunks    = (total + ChunkSize - 1) / ChunkSize; // ceiling division

      std::array<std::array<Enum, ChunkSize>, chunks> arrays{};

      std::size_t index = 0;
      for (std::size_t chunk = 0; chunk < chunks; ++chunk) {
        for (std::size_t i = 0; i < ChunkSize; ++i) {
          if (index < total)
            arrays[chunk][i] = static_cast<Enum>(Min + index++);
        }
      }

      return arrays;
    }
  }

  template<typename Array, typename... Arrays>
  constexpr auto concat_arrays(const Array& first, const Arrays&... rest)
  {
    using T                              = typename Array::value_type;
    constexpr std::size_t     total_size = Array{}.size() + (Arrays{}.size() + ... + 0);
    std::array<T, total_size> result;
    std::size_t               offset = 0;

    for (const auto& elem : first)
      result[offset++] = elem;
    (([&] {
       for (const auto& elem : rest)
         result[offset++] = elem;
     }()),
     ...);

    return result;
  }


} // namespace details


//template<Enum E>
//constexpr std::size_t enum_count = details::enum_count<E>;

template<std::array array_of_enums, typename Pair>
constexpr auto reflect()
{
  constexpr auto expr = std::string_view(details::var_name<array_of_enums>);
  if constexpr (expr.empty()) {
    return std::array<Pair, 0>{};
  }
  else {
    constexpr auto data = details::reflection_data<array_of_enums>();
    return []<auto data>() {
      if constexpr (data.valid_count == 0) {
        return std::array<Pair, 0>{};
      }
      else {
        std::array<Pair, data.valid_count> ret;
        for (std::size_t i = 0, count = 0, ret_index = 0; i < sizeof(data.strings); ++i) {
          if (data.strings[i] == '\0') {
            //const auto& da    = data; // for debugging
            auto& [_, string] = ret[ret_index];
            string            = std::string_view(data.strings + count, i - count);
            count             = i + 1;
            ++ret_index;
          }
        }

        for (std::size_t i = 0, ret_index = 0; i < data.is_valid.size(); ++i) {
          if (data.is_valid[i]) {
            auto& [val, _] = ret[ret_index];
            val            = array_of_enums[i];
            ++ret_index;
          }
        }
        return ret;
      }
    }.operator()<data>();
  }
}

template<Enum E, typename Pair = std::pair<E, std::string_view>>
constexpr auto entries_()
{
  using traits = enum_traits<E>;
  //#if 0
  constexpr auto array_of_array_enums = details::array_of_enums<E, traits::min, traits::max>();
  //  #else
  //  constexpr auto array_of_array_enums_ = details::array_of_enums<std::underlying_type_t<E>, traits::min, traits::max>();
  //  constexpr auto array_of_array_enums  = std::bit_cast <
  //    std::array<std::array<Enum, ENCHANTUM_SEARCH_PER_ITERATION>, array_of_array_enums_.size()>>(array_of_array_enums_);
  //#endif
  return []<std::array A, std::size_t... Idx>(std::index_sequence<Idx...>) {
    return enchantum::details::concat_arrays(enchantum::reflect<A[Idx], Pair>()...);
  }.operator()<array_of_array_enums>(std::make_index_sequence<array_of_array_enums.size()>());
  //  return details::entries_iteration<a, E, Pair>();
}

template<Enum E, typename Pair = std::pair<E, std::string_view>>
inline constexpr auto entries = []() {
  using traits = enum_traits<E>;
  //#if 0
  constexpr auto array_of_array_enums = details::array_of_enums<E, traits::min, traits::max>();
  //  #else
  //  constexpr auto array_of_array_enums_ = details::array_of_enums<std::underlying_type_t<E>, traits::min, traits::max>();
  //  constexpr auto array_of_array_enums  = std::bit_cast <
  //    std::array<std::array<Enum, ENCHANTUM_SEARCH_PER_ITERATION>, array_of_array_enums_.size()>>(array_of_array_enums_);
  //#endif
  return []<std::array A, std::size_t... Idx>(std::index_sequence<Idx...>) {
    return enchantum::details::concat_arrays(enchantum::reflect<A[Idx], Pair>()...);
  }.operator()<array_of_array_enums>(std::make_index_sequence<array_of_array_enums.size()>());
  //  return details::entries_iteration<a, E, Pair>();
}();

template<EnumOfUnderlying<bool> E, typename Pair = std::pair<E, std::string_view>>
inline constexpr auto entries<E, Pair> = enchantum::reflect<std::array{E(false), E(true)}, Pair>();

} // namespace enchantum

#if 0

#pragma once
#include "common.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <string_view>
#include <type_traits>
#include <utility>
//#include <bit> // for bit_cast
namespace enchantum {

namespace details {

  // string view with the simplest code that does not consume any constexpr steps
  struct str_view {};

  template<typename>
  constexpr auto type_name_func() noexcept
  {
    constexpr auto funcname = std::string_view(
      __FUNCSIG__ + (sizeof("auto __cdecl enchantum::details::type_name_func<enum ") - 1));
    // (sizeof("auto __cdecl enchantum::details::type_name_func<") - 1)
    constexpr auto         size = funcname.size() - (sizeof(">(void) noexcept") - 1);
    std::array<char, size> ret;
    auto* const            ret_data      = ret.data();
    const auto* const      funcname_data = funcname.data();
    for (std::size_t i = 0; i < size; ++i)
      ret_data[i] = funcname_data[i];
    return ret;
  }

  //  constexpr bool has_single_valid_enum(const std::size_t      type_name_length,
  //                                       const std::string_view funcsig,
  //                                       std::int64_t           min,
  //                                       std::int64_t           max)
  //  {
  //#define SZC(str) (sizeof(str) - 1)
  //    // IMPORTANT THIS NAME SHOULD MATCH THE FUNCTION BELWO
  //    auto p = SZC("auto __cdecl enchantum::details::var_name_func<class std::array<enum ") + type_name_length +
  //      SZC(",") + numLength(max - min) + SZC(">{enum ") + type_name_length +
  //      ((SZC("(") + SZC("enum ") + type_name_length + SZC(")0x,")) * (max - min)) +
  //      (totalHexDigitsInRange(min, max - 1)) + SZC("}>(void) noexcept") - 1;
  //#undef SZC
  //    return funcsig.size() != p;
  //  }

  template<typename T>
  inline constexpr auto type_name = type_name_func<T>();

  constexpr int numLength(std::uint64_t number)
  {
    int count = 0;
    do {
      count += 1;
      number /= 10;
    } while (number != 0);
    return count;
  }

  constexpr int hexCharCount(std::uint64_t number)
  {
    int count = 1;
    while (number >>= 4) { // shift right by 4 bits (1 hex digit)
      ++count;
    }
    return count;
  }
  constexpr int totalHexDigitsInRange(int64_t start, int64_t end)
  {
    int total = 0;
    for (int64_t i = start; i <= end; ++i) {
      total += hexCharCount(static_cast<uint64_t>(i)); // convert negative to unsigned 2's comp
    }
    return total;
  }

  // explicitly specifiying the calling convention so it is consistent in the __FUNCSIG__
  template<auto V>
  constexpr auto __cdecl var_name_func() noexcept
  {
    constexpr auto type_name_length = type_name<typename decltype(V)::value_type>.size();
    constexpr auto min              = std::int64_t(V.front());
    constexpr auto max              = std::int64_t(V.back());

    // This monster is a way to discard useless long signatures it checks the longest possible
    // signature with all the values being casts because it is most likely and if it is then it discards it
#define SZC(x) (sizeof(x) - 1)
    //constexpr auto Asize      = V.size();
    //constexpr auto numLen     = numLength(Asize);
    //constexpr auto hex        = totalHexDigitsInRange(min, max);
    constexpr auto prefix_len = SZC("auto __cdecl enchantum::details::var_name_func<class std::array<enum ") +
      type_name_length + SZC(",") + numLength(V.size()) + SZC(">{enum ") + type_name_length;

    constexpr auto             siglen = (prefix_len + ((SZC("(enum ") + type_name_length + SZC(")0x,")) * V.size()) +
                             (totalHexDigitsInRange(min, max)) + SZC("}>(void) noexcept") - 1);
    constexpr std::string_view p      = __FUNCSIG__;
    constexpr auto             len    = SZC(__FUNCSIG__);
    if constexpr (len != siglen) {
      return __FUNCSIG__ + prefix_len;
    }
    else {
      return "";
    }
#undef SZC
    //constexpr auto funcname = std::string_view(
    //  __FUNCSIG__ + (sizeof("auto __cdecl enchantum::details::var_name_func<") - 1));
    //// (sizeof("auto __cdecl enchantum::details::type_name_func<") - 1)
    //constexpr auto         size = funcname.size() - (sizeof(">(void) noexcept") - 1);
    //std::array<char, size> ret;
    //auto* const            ret_data      = ret.data();
    //const auto* const      funcname_data = funcname.data();
    //for (std::size_t i = 0; i < size; ++i)
    //  ret_data[i] = funcname_data[i];
    //return ret;
  }


  template<auto V>
  inline constexpr std::string_view var_name = var_name_func<V>();

  constexpr std::size_t search_concat(const std::string_view a, const std::string_view b)
  {
    //constexpr std::string_view colon = "::";
    const std::size_t len_a = a.size();
    const std::size_t len_b = b.size();
    if (len_b + 2 > a.size())
      return a.npos;

    for (std::size_t i = 0; i <= len_a - len_b - 1; ++i)
      if (a.substr(i, len_b) == b)
        return i;
    //const auto next = a.substr(i + len_b, 2);
    // Check for "::"
    //if (next[0] == ')' || next[0] == ':')
    return a.npos;
  }

  template<std::array V>
  constexpr auto reflection_data() noexcept
  {
    constexpr auto funcsig   = std::string_view(details::var_name<V>);
    auto           funcsig_2 = std::string_view(details::var_name<V>);

    constexpr auto funcsig_len     = funcsig.size();
    constexpr auto reflection_data = [name = funcsig]() mutable {
      constexpr const auto& enum_type_name_storage = type_name<typename decltype(V)::value_type>;
      constexpr auto enum_type_name = std::string_view(enum_type_name_storage.data(), enum_type_name_storage.size());
      name.remove_suffix(sizeof(">(void) noexcept") - 1);
      struct {
        char                       buffer[funcsig_len + funcsig_len / 2]{};
        std::array<bool, V.size()> enum_is_valid{};
        std::size_t                valid_count{};
      } ret_val;
      std::size_t buffer_index        = 0;
      std::size_t enum_is_valid_index = 0;

      auto pos = search_concat(name, enum_type_name);
      while (pos != name.npos) {
        if (name[pos + enum_type_name.size()] == ':') { // if it is `EnumName::`
          pos += enum_type_name.size() + 2;             // 2 is :: len
          if (const auto p = name.find(",", pos); p != name.npos) {
            const auto enum_name = name.substr(pos, p - pos);
            for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
              ret_val.buffer[buffer_index + i] = enum_name[i];

            buffer_index += enum_name.size();
            buffer_index += 1;
            name.remove_prefix(p + 1); // one past comma
          }
          else {
            // Text is
            // A::Val} <- need to remove this bracket
            auto enum_name = name.substr(pos);
            enum_name.remove_suffix(1);
            for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
              ret_val.buffer[buffer_index + i] = enum_name[i];

            buffer_index += enum_name.size();
            name = std::string_view{}; // end of string comma
          }
          ++ret_val.valid_count;
          ret_val.enum_is_valid[enum_is_valid_index] = true;
        }
        else {
          name.remove_prefix(pos + enum_type_name.size() + 1); // skip pos and the typename and the ')'
        }
        if (enum_is_valid_index >= ret_val.enum_is_valid.size())
          return ret_val;
        ++enum_is_valid_index;


        pos = search_concat(name, enum_type_name);
      }
      return ret_val;
    }();

    return [reflection_data]() {
      constexpr auto min_length = [reflection_data]() {
        std::size_t len = 0;
        for (std::size_t i = 0; i < sizeof(reflection_data.buffer); ++i) {
          if (reflection_data.buffer[i] == '\0' && reflection_data.buffer[i + 1] == '\0')
            return len + 1;
          ++len;
        }
        return len;
      }();
      struct {
        char                       strings[min_length];
        std::array<bool, V.size()> is_valid;
        std::size_t                valid_count;
      } data;
      for (std::size_t i = 0; i < min_length; ++i) {
        data.strings[i] = reflection_data.buffer[i];
      }
      data.valid_count = reflection_data.valid_count;
      data.is_valid    = reflection_data.enum_is_valid;
      return data;
    }();
  }

  template<auto V>
  inline constexpr auto static_var = reflection_data<V>();

  template<typename Enum, auto Min, auto Max>
  constexpr auto array_of_enums()
  {
    if constexpr (BitFlagEnum<Enum>) {
      using T                                    = std::underlying_type_t<Enum>;
      constexpr std::size_t                 bits = sizeof(T) * CHAR_BIT;
      std::array<std::array<Enum, bits>, 1> a{};
      for (std::size_t i = 0; i < bits; ++i)
        a[0][i] = static_cast<Enum>(static_cast<std::make_unsigned_t<T>>(1) << i);
      return a;
    }
    else {
      // + shuts down stupid "warning C4804: '<': unsafe use of type 'bool' in operation"
      static_assert(Min < Max, "Min must be less than Max");

      constexpr auto        ChunkSize = ENCHANTUM_SEARCH_PER_ITERATION;
      constexpr std::size_t total     = Max - Min;
      constexpr std::size_t chunks    = (total + ChunkSize - 1) / ChunkSize; // ceiling division

      std::array<std::array<Enum, ChunkSize>, chunks> arrays{};

      std::size_t index = 0;
      for (std::size_t chunk = 0; chunk < chunks; ++chunk) {
        for (std::size_t i = 0; i < ChunkSize; ++i) {
          if (index < total)
            arrays[chunk][i] = static_cast<Enum>(Min + index++);
        }
      }

      return arrays;
    }
  }

  template<typename Array, typename... Arrays>
  constexpr auto concat_arrays(const Array& first, const Arrays&... rest)
  {
    using T                              = typename Array::value_type;
    constexpr std::size_t     total_size = Array{}.size() + (Arrays{}.size() + ... + 0);
    std::array<T, total_size> result;
    std::size_t               offset = 0;

    for (const auto& elem : first)
      result[offset++] = elem;
    (([&] {
       for (const auto& elem : rest)
         result[offset++] = elem;
     }()),
     ...);

    return result;
  }


} // namespace details


//template<Enum E>
//constexpr std::size_t enum_count = details::enum_count<E>;

template<std::array array_of_enums, typename Pair>
constexpr auto reflect()
{
  constexpr auto expr = std::string_view(details::var_name<array_of_enums>);
  if constexpr (expr.empty()) {
    return std::array<Pair, 0>{};
  }
  else {
    constexpr auto data = details::reflection_data<array_of_enums>();
    return []<auto data>() {
      if constexpr (data.valid_count == 0) {
        return std::array<Pair, 0>{};
      }
      else {
        std::array<Pair, data.valid_count> ret;
        for (std::size_t i = 0, count = 0, ret_index = 0; i < sizeof(data.strings); ++i) {
          if (data.strings[i] == '\0') {
            //const auto& da    = data; // for debugging
            auto& [_, string] = ret[ret_index];
            string            = std::string_view(data.strings + count, i - count);
            count             = i + 1;
            ++ret_index;
          }
        }

        for (std::size_t i = 0, ret_index = 0; i < data.is_valid.size(); ++i) {
          if (data.is_valid[i]) {
            auto& [val, _] = ret[ret_index];
            val            = array_of_enums[i];
            ++ret_index;
          }
        }
        return ret;
      }
    }.operator()<data>();
  }
}

template<Enum E, typename Pair = std::pair<E, std::string_view>>
inline constexpr auto entries = []() {
  using traits = enum_traits<E>;
  //#if 0
  constexpr auto array_of_array_enums = details::array_of_enums<E, traits::min, traits::max>();
  //  #else
  //  constexpr auto array_of_array_enums_ = details::array_of_enums<std::underlying_type_t<E>, traits::min, traits::max>();
  //  constexpr auto array_of_array_enums  = std::bit_cast <
  //    std::array<std::array<Enum, ENCHANTUM_SEARCH_PER_ITERATION>, array_of_array_enums_.size()>>(array_of_array_enums_);
  //#endif
  return []<std::array A, std::size_t... Idx>(std::index_sequence<Idx...>) {
    return enchantum::details::concat_arrays(enchantum::reflect<A[Idx], Pair>()...);
  }.operator()<array_of_array_enums>(std::make_index_sequence<array_of_array_enums.size()>());
  //  return details::entries_iteration<a, E, Pair>();
}();

} // namespace enchantum
#endif