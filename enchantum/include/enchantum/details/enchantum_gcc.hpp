#include "../common.hpp"
#include "generate_arrays.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <type_traits>
#include <utility>

#include "string_view.hpp"

namespace enchantum {

namespace details {


  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename _>
  constexpr auto type_name_func() noexcept
  {
    // constexpr auto f() [with _ = Scoped]
    //return __PRETTY_FUNCTION__;
    constexpr auto funcname = string_view(
      __PRETTY_FUNCTION__ + (sizeof("constexpr auto enchantum::details::type_name_func() [with _ = ") - 1));
    // (sizeof("auto __cdecl enchantum::details::type_name_func<") - 1)
    constexpr auto         size = funcname.size() - (sizeof("]") - 1);
    std::array<char, size> ret;
    auto* const            ret_data      = ret.data();
    const auto* const      funcname_data = funcname.data();
    for (std::size_t i = 0; i < size; ++i)
      ret_data[i] = funcname_data[i];
    return ret;
  }

  template<auto Enum>
  constexpr auto enum_in_array_name() noexcept
  {
    // constexpr auto f() [with auto _ = (
    //constexpr auto f() [with auto _ = (Scoped)0]
    string_view s = __PRETTY_FUNCTION__ +
      sizeof("constexpr auto enchantum::details::enum_in_array_name() [with auto Enum = ") - 1;
    s.remove_suffix(sizeof("]") - 1);

    if constexpr (ScopedEnum<decltype(Enum)>) {
      if (s.front() == '(') {
        s.remove_prefix(1);
        s.remove_suffix(sizeof(")0") - 1);
        return s;
      }
      return s.substr(0, s.rfind("::"));
    }
    else {
      if (s.front() == '(') {
        s.remove_prefix(1);
        s.remove_suffix(sizeof(")0") - 1);
      }
      if (const auto pos = s.rfind("::"); pos != s.npos)
        return s.substr(0, pos);
      return string_view();
    }
  }

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting() noexcept
  {
    if constexpr (ScopedEnum<Enum>) {
      constexpr auto s = details::enum_in_array_name<Enum{}>();
      return s.substr(0, s.rfind("::")).size();
    }
    else {
      constexpr auto  s      = enum_in_array_name<Enum{}>().size();
      constexpr auto& tyname = static_storage_for<type_name_func<Enum>()>;
      constexpr auto  str    = string_view(tyname.data(), tyname.size());
      if (constexpr auto pos = str.rfind("::"); pos != str.npos) {
        return s + str.substr(pos).size();
      }
      else {
        return s + tyname.size();
      }
    }
  }

  template<auto Array>
  constexpr auto var_name() noexcept
  {
    //constexpr auto f() [with auto _ = std::array<E, 6>{std::__array_traits<E, 6>::_Type{a, b, c, e, d, (E)6}}]

    using T = typename decltype(Array)::value_type;
#define SZC(x) (sizeof(x) - 1)
    std::size_t    funcsig_off   = SZC("constexpr auto enchantum::details::var_name() [with auto Array = std::array<");
    constexpr auto type_name_len = enchantum::details::type_name_func<T>().size();
    funcsig_off += 2 * (type_name_len + SZC(" ,"));
    funcsig_off += SZC(">{std::__array_traits<");
    funcsig_off += SZC(">::_Type{");
    constexpr auto Size = Array.size();
    // clang-format off
	funcsig_off += 2 * (Size < 10 ? 1
			: Size < 100 ? 2
			: Size < 1000 ? 3
			: Size < 10000 ? 4
			: Size < 100000 ? 5
			: Size < 1000000 ? 6
			: Size < 10000000 ? 7
			: Size < 100000000 ? 8
			: Size < 1000000000 ? 9
			: 10);
    // clang-format on
    return string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - (sizeof("}}]") - 1));
  }


  // 10.140
  //  9.988
  template<typename E, typename Pair, auto Min, auto Max>
  constexpr auto reflect() noexcept
  {
    constexpr auto elements = []() {
      constexpr auto length_of_enum_in_template_array_casting = details::length_of_enum_in_template_array_if_casting<E>();
      constexpr auto Array = details::generate_arrays<E, Min, Max>();
      auto           str   = var_name<Array>();
      struct RetVal {
        std::array<Pair, Array.size()> pairs{};
        std::size_t                    total_string_length = 0;
        std::size_t                    valid_count         = 0;
      } ret;
      std::size_t    index             = 0;
      constexpr auto enum_in_array_len = details::enum_in_array_name<E{}>().size();
      while (index < Array.size()) {
        if (str.front() == '(') {
          str.remove_prefix(sizeof("(") - 1 + length_of_enum_in_template_array_casting + sizeof(")0") -
                            1); // there is atleast 1 base 10 digit
          //if(!str.empty())
          //	std::cout << "after str \"" << str << '"' << '\n';

          if (const auto commapos = str.find(','); commapos != str.npos)
            str.remove_prefix(commapos + 2);

          //std::cout << "strsize \"" << str.size() << '"' << '\n';
        }
        else {
          if constexpr (enum_in_array_len != 0) {
            str.remove_prefix(enum_in_array_len + sizeof("::") - 1);
          }
          const auto commapos = str.find(',');

          const auto name = str.substr(0, commapos);

          ret.pairs[ret.valid_count] = Pair{Array[index], name};
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
      for (std::size_t _i = 0; _i < elements.valid_count; ++_i) {
        const auto& [_, s] = elements.pairs[_i];
        for (std::size_t i = 0; i < s.size(); ++i)
          strings[index++] = s[i];
        ++index;
      }
      return strings;
    }();

    std::array<Pair, elements.valid_count> ret;
    std::size_t                            string_index = 0;
    constexpr const auto*                  str          = static_storage_for<strings>.data();
    for (std::size_t i = 0; i < elements.valid_count; ++i) {
      const auto& [e, s] = elements.pairs[i];
      auto& [re, rs]     = ret[i];
      re                 = e;
      rs                 = {str + string_index, str + string_index + s.size()};
      string_index += s.size() + 1;
    }
    return ret;
  }

} // namespace details

} // namespace enchantum
