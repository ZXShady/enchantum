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

  template<typename T>
  inline constexpr auto type_name_unscoped_enum = []() {
    constexpr auto name = std::string_view(type_name<T>.data(), type_name<T>.size());
    if constexpr ([[maybe_unused]] const auto pos = name.rfind("::"); pos != name.npos) // may not be used in all branches
      return name.substr(0, pos);
    else
      return std::string_view();
  }();

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


  template<int SizeOfUnderlyingType>
  constexpr int totalHexDigitsInRange(int64_t start, int64_t end)
  {
    int total = 0;
    for (int64_t i = start; i <= end; ++i) {
      if constexpr (SizeOfUnderlyingType == 1)
        total += hexCharCount(static_cast<std::uint8_t>(i)); // convert negative to unsigned 2's comp
      else if constexpr (SizeOfUnderlyingType == 2)
        total += hexCharCount(static_cast<std::uint16_t>(i)); // convert negative to unsigned 2's comp
      else if constexpr (SizeOfUnderlyingType == 4 || SizeOfUnderlyingType == 8)
        total += hexCharCount(static_cast<std::uint64_t>(i)); // convert negative to unsigned 2's comp
      else
        static_assert(SizeOfUnderlyingType == 1, "Invalid SizeOfUnderlyingType");
    }
    return total;
  }

  // explicitly specifiying the calling convention so it is consistent in the __FUNCSIG__
  template<auto V>
  constexpr auto __cdecl var_name_func() noexcept
  {
    //"auto __cdecl enchantum::details::var_name_func<class std::array<enum UC,32>{enum UC(enum UC)0xe0,(enum "
    //"UC)0xe1,(enum UC)0xe2,(enum UC)0xe3,(enum UC)0xe4,(enum UC)0xe5,(enum UC)0xe6,(enum UC)0xe7,(enum UC)0xe8,(enum "
    //"UC)0xe9,(enum UC)0xea,(enum UC)0xeb,(enum UC)0xec,(enum UC)0xed,(enum UC)0xee,(enum UC)0xef,(enum UC)0xf0,(enum "
    //"UC)0xf1,(enum UC)0xf2,(enum UC)0xf3,(enum UC)0xf4,(enum UC)0xf5,(enum UC)0xf6,(enum UC)0xf7,(enum UC)0xf8,(enum "
    //"UC)0xf9,(enum UC)0xfa,(enum UC)0xfb,(enum UC)0xfc,(enum UC)0xfd,(enum UC)0xfe,(enum UC)0xff}>(void) noexcept";
#define SZC(x) (sizeof(x) - 1)

    using T                   = typename decltype(V)::value_type;
    constexpr auto  type_name = details::type_name<T>;
    constexpr auto& array     = V;
    constexpr auto  min       = std::int64_t(V.front());
    constexpr auto  max       = std::int64_t(V.back());

    constexpr auto name_prefix_len    = SZC("auto __cdecl enchantum::details::var_name_func<class std::array<enum ");
    constexpr auto type_name_in_array = type_name.size();
    constexpr auto comma_in_std_array = SZC(",");
    constexpr auto std_array_size     = numLength(array.size());
    constexpr auto array_member_type_name  = SZC(">{enum ") + type_name.size();
    constexpr auto array_values_type_names = (SZC("(enum ") + type_name.size() + SZC(")0x")) * V.size();
    constexpr auto commas_count            = SZC(",") * V.size() - 1;
    constexpr auto total_hex_numbers       = totalHexDigitsInRange<sizeof(T)>(min, max);
    constexpr auto name_suffix_len         = SZC("}>(void) noexcept");


    constexpr auto total = name_prefix_len + type_name_in_array + comma_in_std_array + std_array_size +
      array_member_type_name + array_values_type_names + commas_count + total_hex_numbers + name_suffix_len;
    constexpr auto len      = SZC(__FUNCSIG__);
    //constexpr auto _funcsig = __FUNCSIG__;
    if constexpr (len == total)
      return "";
    else
      return __FUNCSIG__ + name_prefix_len + type_name_in_array + comma_in_std_array + std_array_size +
        array_member_type_name;

#if 0
    using T                  = typename decltype(V)::value_type;
    constexpr auto type_nam  = type_name<T>;
    constexpr auto type_nam2 = __FUNCSIG__;

    constexpr auto type_name_length = type_name<T>.size();



    // This monster is a way to discard useless long signatures it checks the longest possible
    // signature with all the values being casts because it is most likely and if it is then it discards it
  #define SZC(x) (sizeof(x) - 1)
    //constexpr auto Asize      = V.size();
    //constexpr auto numLen     = numLength(Asize);
    //constexpr auto hex        = totalHexDigitsInRange(min, max);
    constexpr auto numLen     = numLength(V.size());
    constexpr auto prefix_len = SZC("auto __cdecl enchantum::details::var_name_func<class std::array<enum ") +
      type_name_length + SZC(",") + numLength(V.size()) + SZC(">{enum ") + type_name_length;

    const auto     p      = totalHexDigitsInRange<sizeof(T)>(min, max);
    constexpr auto siglen = prefix_len 
        + ((SZC("(enum ") + type_name_length + SZC(")0x,")) * V.size()) 
        + (totalHexDigitsInRange<sizeof(T)>(min, max)) + SZC("}>(void) noexcept");

    constexpr auto _prefix  = ((SZC("(enum ") + type_name_length + SZC(")0x,")) * V.size());
    constexpr auto _prefix2 = totalHexDigitsInRange<sizeof(T)>(min, max) + SZC("}>(void) noexcept");
    
    constexpr auto len    = SZC(__FUNCSIG__);
    if constexpr (len != siglen) {
      return __FUNCSIG__ + prefix_len;
    }
    else {
      return "";
    }
#endif
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

  constexpr std::size_t search_for_scoped_enums(const std::string_view funcsig,
                                                const std::string_view enum_name,
                                                std::size_t&           skipped)
  {
    constexpr std::string_view colon = "::";
    std::size_t                pos1  = 0;
    while ((pos1 = funcsig.find(enum_name, pos1)) != std::string_view::npos) {
      // After finding str1, check if str2 follows it immediately
      if (funcsig.substr(pos1 + enum_name.size()).starts_with(colon))
        return pos1 + enum_name.size() + colon.size(); // 2 is "::" length
      ++skipped;
      pos1 += enum_name.size(); // we know we are in an array text
    }
    return std::string_view::npos;
  }
#if 0
  constexpr std::size_t search_for_scoped_enums(const std::string_view a, const std::string_view b, std::size_t& skipped)
  {
    constexpr std::string_view colon = "::";
    const std::size_t          len_a = a.size();
    const std::size_t          len_b = b.size();
    if (len_b + 2 > a.size())
      return a.npos;

    for (std::size_t i = 0; i <= len_a - len_b - 1; ++i)
      if (a.substr(i, len_b) == b && a.substr(i + len_b).starts_with(colon))
        return i + len_b + colon.size();
    //const auto next = a.substr(i + len_b, 2);
    // Check for "::"
    //if (next[0] == ')' || next[0] == ':')
    return a.npos;
  }
#endif
  template<int SizeOfUnderlyingType>
  constexpr std::size_t search_for_unscoped_enums(const std::string_view s,
                                                  const std::string_view full_type_name,
                                                  //std::int64_t           starting_enum_value,
                                                  std::size_t&           skipped)
  {
    std::size_t pos = 0;
    while (true) { // this assumes that the passed string 's' contains ATLEAST 1 valid enum otherwise infinite loops
      if (pos >= s.size())
        return s.npos;
      if (s[pos] == '(') {
        pos += full_type_name.length() + (sizeof("(enum )") - 1);
        //std::size_t i = pos;
        //for (const auto size = s.size(); i < size; ++i)
        //  if (s[i] == ',')
        //    break;
        //        auto s = SizeOfUnderlyingType;
        //const auto i = starting_enum_value + static_cast<std::int64_t>(skipped);
        pos += sizeof("0x,") - 1;
        while (pos < s.size() && s[pos] != ',')
          ++pos;
        ++pos; // skip the comma
        ++skipped;
        //if constexpr (SizeOfUnderlyingType == 1)
        //  pos += 2; //hexCharCount(static_cast<std::uint8_t>(i)); // convert negative to unsigned 2's comp
        //else if constexpr (SizeOfUnderlyingType == 2)
        //  pos += hexCharCount(static_cast<std::uint16_t>(i)); // convert negative to unsigned 2's comp
        //else if constexpr (SizeOfUnderlyingType == 4 || SizeOfUnderlyingType == 8)
        //  pos += hexCharCount(static_cast<std::uint64_t>(i)); // convert negative to unsigned 2's comp
        //else
        //  static_assert(SizeOfUnderlyingType == 1, "Invalid SizeOfUnderlyingType");
        //if constexpr (SizeOfUnderlyingType == 1)
        //  pos += sizeof("0xff") - 1;
        //if constexpr (SizeOfUnderlyingType == 2)
        //  pos += sizeof("0xffff") - 1;
        //if constexpr (SizeOfUnderlyingType == 4 || SizeOfUnderlyingType == 8)
        //  pos += sizeof("0xffffffffffffffff") - 1;
        //else
        //  static_assert(SizeOfUnderlyingType == 1);
        //string.remove_prefix(pos - oldp);
      }
      else {
        return pos;
      }
    }
    // this assumes that the passed string 's' contains ATLEAST 1 valid enum otherwise infinite loops
  }

  template<std::array V>
  constexpr auto parse_reflection_data()
  {
    using Enum               = typename decltype(V)::value_type;
    constexpr auto funcsig   = std::string_view(details::var_name<V>);
    //auto           funcsig_2 = std::string_view(details::var_name_func<V>());
    auto name = funcsig;

    constexpr const auto enum_type_name = true
      ? std::string_view(type_name<Enum>.data(), type_name<Enum>.size())
      : type_name_unscoped_enum<Enum>;
    name.remove_suffix(sizeof("}>(void) noexcept") - 1);
    struct {
      char                       buffer[funcsig.size()]{};
      std::array<bool, V.size()> enum_is_valid{};
      std::size_t                valid_count{};
    } ret_val;
    std::size_t buffer_index = 0;

    std::size_t                     skipped             = 0;
    std::size_t                     pos                 = 0;
    //[[maybe_unused]] constexpr auto starting_enum_value = static_cast<std::int64_t>(V.front());

    if constexpr (UnscopedEnum<Enum>) // && !BitFlagEnum<Enum>)
      pos = search_for_unscoped_enums<sizeof(Enum)>(name, enum_type_name /*, starting_enum_value*/, skipped);
    else
      pos = search_for_scoped_enums(name, enum_type_name, skipped);
    while (pos != name.npos) {
      constexpr auto skip_enum_prefix_count = []() -> std::size_t {
        if constexpr (ScopedEnum<Enum>)
          return 0;
        else if constexpr (type_name_unscoped_enum<Enum>.size() != 0)
          return (sizeof("::") - 1) + type_name_unscoped_enum<Enum>.size();
        else
          return 0;
      }();
      if (const auto comma = name.find(",", pos + skip_enum_prefix_count); comma != name.npos) {
        const auto enum_name = name.substr(pos + skip_enum_prefix_count, comma - (pos + skip_enum_prefix_count));
        for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
          ret_val.buffer[buffer_index + i] = enum_name[i];

        buffer_index += enum_name.size() + 1; // skip comma
        name.remove_prefix(comma + 1);        // one past comma
      }
      else {
        // Text is
        // A::Val}
        auto enum_name = name.substr(pos + skip_enum_prefix_count);
        //enum_name.remove_suffix(1);
        for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
          ret_val.buffer[buffer_index + i] = enum_name[i];

        buffer_index += enum_name.size();
        name = std::string_view{}; // end of string
      }
      ++ret_val.valid_count;

      if (skipped >= V.size())
        break;

      ret_val.enum_is_valid[skipped] = true;
      if constexpr (ScopedEnum<Enum>)
        pos = search_for_scoped_enums(name, enum_type_name, skipped);
      else
        pos = search_for_unscoped_enums<sizeof(Enum)>(name, enum_type_name /*, starting_enum_value*/, skipped);
      // The string is shortened by `.remove_prefix` therefore we need to account for the element passed
      ++skipped;
    }
    return ret_val;
  }

  template<std::array V>
  constexpr auto reflection_data() noexcept
  {
    using Enum = typename decltype(V)::value_type;


    constexpr auto reflection_data  = parse_reflection_data<V>();
    //auto           reflection_data_ = parse_reflection_data<V>();


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
      static_assert(Min < Max, "enum_traits::min must be less than enum_traits::max");

      constexpr auto        ChunkSize = ENCHANTUM_SEARCH_PER_ITERATION;
      constexpr std::size_t total     = Max - Min;
      constexpr std::size_t chunks    = (total + ChunkSize - 1) / ChunkSize; // ceiling division

      std::array<std::array<Enum, ChunkSize>, chunks> arrays{};

      std::size_t index = 0;
      for (std::size_t chunk = 0; chunk < chunks; ++chunk) {
        for (std::size_t i = 0; i < ChunkSize; ++i) {
          arrays[chunk][i] = static_cast<Enum>(Min + index);
          ++index;
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
  auto           _    = std::string_view(details::var_name_func<array_of_enums>());
  if constexpr (expr.empty()) {
    return std::array<Pair, 0>{};
  }
  else {
    constexpr auto data    = details::reflection_data<array_of_enums>();
    //auto           _2_data = details::reflection_data<array_of_enums>();

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
    }.template operator()<data>();
  }
}

template<Enum E, typename Pair = std::pair<E, std::string_view>>
inline constexpr auto entries = []() {
  static_assert(sizeof(E) <= sizeof(std::int64_t),
                "enchantum does not support enums with underlying types larger than std::int64_t");
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
  }.template operator()<array_of_array_enums>(std::make_index_sequence<array_of_array_enums.size()>());
  //  return details::entries_iteration<a, E, Pair>();
}();

template<EnumOfUnderlying<bool> E, typename Pair>
inline constexpr auto entries<E, Pair> = enchantum::reflect<std::array{E(false), E(true)}, Pair>();

} // namespace enchantum
