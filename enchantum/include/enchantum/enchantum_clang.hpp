#include "common.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <string_view>
#include <type_traits>
#include <utility>

#include "details/string_view.hpp"


namespace enchantum {
namespace details {
  template<typename T, std::underlying_type_t<T> max_range = 1>
  constexpr auto valid_cast_range()
  {
    if constexpr (max_range >= 0) {
      if constexpr (max_range < ENCHANTUM_MAX_RANGE) {
        // this tests whether `static_cast`ing max_range is valid
        // because C style enums stupidly is like a bit field
        // `enum E { a,b,c,d = 3};` is like a bitfield `struct E { int val : 2;}`
        // which means giving E.val a larger than 2 bit value is UB so is it for enums
        // and gcc and msvc ignore this (for good)
        // while clang makes it a subsituation failure which we can check for
        // using std::inegral_constant makes sure this is a constant expression situation
        // for SFINAE to occur
        if constexpr (requires { std::integral_constant<T, static_cast<T>(max_range)>{}; })
          return valid_cast_range<T, max_range * 2>();
        else
          return max_range - 1;
      }
      else {
        return max_range - 1;
      }
    }
    else {
      if constexpr (max_range > ENCHANTUM_MIN_RANGE) {
        // this tests whether `static_cast`ing max_range is valid
        // because C style enums stupidly is like a bit field
        // `enum E { a,b,c,d = 3};` is like a bitfield `struct E { int val : 2;}`
        // which means giving E.val a larger than 2 bit value is UB so is it for enums
        // and gcc and msvc ignore this (for good)
        // while clang makes it a subsituation failure which we can check for
        // using std::inegral_constant makes sure this is a constant expression situation
        // for SFINAE to occur
        if constexpr (requires { std::integral_constant<T, static_cast<T>(max_range)>{}; })
          return valid_cast_range<T, max_range * 2>();
        else
          return max_range / 2;
      }
      else {
        return max_range / 2;
      }
    }
  }
} // namespace details

template<UnscopedEnum E>
  requires SignedEnum<E> // for concept subsumption rules
struct enum_traits<E> {
  static constexpr auto          max = details::valid_cast_range<E>();
  static constexpr decltype(max) min = details::valid_cast_range<E, -1>();
};

template<UnscopedEnum E>
  requires UnsignedEnum<E> // for concept subsumption rules
struct enum_traits<E> {
  static constexpr auto          max = details::valid_cast_range<E>();
  static constexpr decltype(max) min = 0;
};

namespace details {

  // string view with the simplest code that does not consume any constexpr steps
  struct str_view {};

  template<typename _>
  constexpr auto type_name_func() noexcept
  {
    // constexpr auto f() [with _ = Scoped]
    //return __PRETTY_FUNCTION__;
    constexpr auto funcname = std::string_view(
      __PRETTY_FUNCTION__ + (sizeof("auto enchantum::details::type_name_func() [_ = ") - 1));
    // (sizeof("auto __cdecl enchantum::details::type_name_func<") - 1)
    constexpr auto         size = funcname.size() - (sizeof("]") - 1);
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


  template<auto Enum>
  constexpr auto enum_in_array_name() noexcept
  {
    // constexpr auto f() [with auto _ = (
    //constexpr auto f() [Enum = (Scoped)0]
    std::string_view s = __PRETTY_FUNCTION__ + (sizeof("auto enchantum::details::enum_in_array_name() [Enum = ") - 1);
    s.remove_suffix(sizeof("]") - 1);

    if constexpr (ScopedEnum<decltype(Enum)>) {
      if (s[s.size() - 2] == ')') {
        s.remove_prefix(sizeof("(") - 1);
        s.remove_suffix(sizeof(")0") - 1);
        return s;
      }
      else {
        return s.substr(0, s.rfind("::"));
      }
    }
    else {
      if (s[s.size() - 2] == ')') {
        s.remove_prefix(sizeof("(") - 1);
        s.remove_suffix(sizeof(")0") - 1);
      }
      if (const auto pos = s.rfind("::"); pos != s.npos)
        return s.substr(0, pos);
      return std::string_view();
    }
  }

  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    // "auto enchantum::details::var_name() [Vs = <(A)0, a, b, c, e, d, (A)6>]"
#define SZC(x) (sizeof(x) - 1)
    constexpr auto funcsig_off = SZC("auto enchantum::details::var_name() [Vs = <");
    return std::string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - SZC(">]"));
#undef SZC
  }


  template<typename T, auto Min, decltype(Min) Max>
  constexpr auto generate_arrays()
  {
    using Enum = T;
    if constexpr (BitFlagEnum<Enum>) {
      constexpr std::size_t  bits = sizeof(T) * CHAR_BIT;
      std::array<Enum, bits> a{};
      for (std::size_t i = 0; i < bits; ++i)
        a[i] = static_cast<Enum>(static_cast<std::make_unsigned_t<T>>(1) << i);
      return a;
    }
    else {
      static_assert(Min < Max, "enum_traits::min must be less than enum_traits::max");
      std::array<Enum, (Max - Min) + 1> array;
      auto* const                       array_data = array.data();
      for (std::size_t i = 0; i < array.size(); ++i)
        array_data[i] = static_cast<Enum>(static_cast<decltype(Min)>(i) + Min);
      return array;
    }
  }

  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, typename Pair, auto Min, auto Max>
  constexpr auto reflect() noexcept
  {
    constexpr auto elements = []() {
      constexpr auto Array = details::generate_arrays<E, Min, Max>();
      auto str             = [Array]<std::size_t... Idx>(std::index_sequence<Idx...>) {
        return var_name<Array[Idx]...>();
      }(std::make_index_sequence<Array.size()>());

      struct RetVal {
        std::array<Pair, decltype(Array){}.size()> pairs;
        std::size_t                                total_string_length = 0;
        std::size_t                                valid_count         = 0;
      } ret;

      std::size_t    index              = 0;
      constexpr auto enum_in_array_name = details::enum_in_array_name<E{}>();
      constexpr auto enum_in_array_len  = enum_in_array_name.size();

      // ((anonymous namespace)::A)0
      // (anonymous namespace)::a

      // this is needed to determine whether the above are cast expression if 2 braces are
      // next to eachother then it is a cast but only for anonymoused namespaced enums
      constexpr std::size_t index_check = !enum_in_array_name.empty() && enum_in_array_name.front() == '(' ? 1 : 0;
      while (index < Array.size()) {
        if (str[index_check] == '(') {
          str.remove_prefix(sizeof("(") - 1 + enum_in_array_len + sizeof(")0") - 1); // there is atleast 1 base 10 digit
          //if(!str.empty())
          //	std::cout << "after str \"" << str << '"' << '\n';
          if (const auto commapos = str.find(","); commapos != str.npos)
            str.remove_prefix(commapos + 2);
          //std::cout << "strsize \"" << str.size() << '"' << '\n';
        }
        else {
          str.remove_prefix(enum_in_array_len);
          if constexpr (enum_in_array_len != 0)
            str.remove_prefix(sizeof("::") - 1);
          const auto commapos = str.find(",");

          const auto name = str.substr(0, commapos);

          ret.pairs[index] = Pair{E(Array[index]), name};
          ret.total_string_length += name.size() + 1;

          if (commapos != str.npos)
            str.remove_prefix(commapos + 2);
          ++ret.valid_count;
        }
        ++index;
      }
      return ret;
    }();

    constexpr auto strings = [elements]() {
      std::array<char, elements.total_string_length> strings{};
      std::size_t                                    index = 0;
      for (const auto& [_, s] : elements.pairs) {
        if (s.empty())
          continue;

        for (std::size_t i = 0; i < s.size(); ++i)
          strings[index++] = s[i];
        ++index;
      }
      return strings;
    }();

    std::array<Pair, elements.valid_count> ret;
    std::size_t                            string_index    = 0;
    std::size_t                            string_index_to = 0;

    std::size_t     index = 0;
    constexpr auto& str   = static_storage_for<strings>;
    for (auto& [e, s] : elements.pairs) {
      if (s.empty())
        continue;
      auto& [re, rs] = ret[index++];
      re             = e;
      for (std::size_t i = string_index; i < str.size(); ++i) {
        string_index_to = i;
        if (str[i] == '\0')
          break;
      }
      rs           = {&str[string_index], &str[string_index_to]};
      string_index = string_index_to + 1;
    }
    return ret;
  }

} // namespace details


//template<Enum E>
//constexpr std::size_t enum_count = details::enum_count<E>;


} // namespace enchantum
