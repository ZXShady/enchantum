#include <array>
#include <bit>
#include <cassert>
#include <climits>
#include <string_view>
#include <type_traits>
#include <utility>
#include "common.hpp"

namespace enchantum {

namespace details {

  // string view with the simplest code that does not consume any constexpr steps
  struct str_view {};

  template<typename _>
  constexpr auto type_name_func() noexcept
  {
    // constexpr auto f() [with _ = Scoped]
    //return __PRETTY_FUNCTION__;
    constexpr auto funcname = std::string_view(
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
    //constexpr auto f() [with auto _ = (Scoped)0]
    std::string_view s = __PRETTY_FUNCTION__ +
      sizeof("constexpr auto enchantum::details::enum_in_array_name() [with auto Enum = ") - 1;
    s.remove_suffix(sizeof("]") - 1);

    //
    if constexpr (ScopedEnum<decltype(Enum)>) {
      if (s.front() == '(') {
        s.remove_prefix(1);
        s.remove_suffix(sizeof(")0") - 1);
        return s;
      }
      else {
        return s.substr(0, s.rfind("::"));
      }
    }
    else {
      if (s.front() == '(') {
        s.remove_prefix(1);
        s.remove_suffix(sizeof(")0") - 1);
      }
      if (const auto pos = s.rfind("::"); pos != s.npos)
        return s.substr(0, pos);
      return std::string_view();
    }
  }

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting() noexcept
  {
    if constexpr (ScopedEnum<Enum>) {
      constexpr auto s = enum_in_array_name<Enum{}>();
      return s.substr(0, s.rfind("::")).size();
    }
    else {
      constexpr auto  s      = enum_in_array_name<Enum{}>().size();
      constexpr auto& tyname = type_name<Enum>;
      constexpr auto  str    = std::string_view(tyname.data(), tyname.size());
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
    constexpr auto type_name_len = enchantum::details::type_name<T>.size();
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
    return std::string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - (sizeof("}}]") - 1));
  }


  template<typename T, bool IsBitFlag, auto Min, decltype(Min) Max>
  constexpr auto generate_arrays()
  {
    using Enum = T;
    if constexpr (IsBitFlag) {
      constexpr std::size_t  bits = sizeof(T) * CHAR_BIT;
      std::array<Enum, bits> a{};
      for (std::size_t i = 0; i < bits; ++i)
        a[i] = static_cast<Enum>(static_cast<std::make_unsigned_t<T>>(1) << i);
      return a;
    }
    else {
      static_assert(Min < Max, "enum_traits::min must be less than enum_traits::max");
      std::array<Enum, (Max - Min) + 1> array;
      auto* const                 array_data = array.data();
      for (std::size_t i = 0; i < array.size(); ++i)
        array_data[i] = static_cast<Enum>(i + Min);
      return array;
    }
  }

  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, typename Pair, auto Min, auto Max>
  constexpr auto reflect() noexcept
  {
    constexpr auto elements = []() {
      constexpr auto length_of_enum_in_template_array_casting = length_of_enum_in_template_array_if_casting<E>();
      constexpr auto Array = generate_arrays<std::underlying_type_t<E>, BitFlagEnum<E>, Min, Max>();
      using EnumArray      = std::array<E, Array.size()>;
      auto str             = var_name<std::bit_cast<EnumArray>(Array)>();
      struct RetVal {
        std::array<Pair, Array.size()> pairs;
        std::size_t                    total_string_length = 0;
        std::size_t                    valid_count         = 0;
      } ret;
      std::size_t    index             = 0;
      constexpr auto enum_in_array_len = enum_in_array_name<E{}>().size();
      while (index < Array.size()) {
        if (str.front() == '(') {
          str.remove_prefix(sizeof("(") - 1 + length_of_enum_in_template_array_casting + sizeof(")0") -
                            1); // there is atleast 1 base 10 digit
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

    //constexpr auto elements = []() {
    //	constexpr auto length_of_enum_in_template_array_casting = length_of_enum_in_template_array_if_casting<E>();
    //	constexpr auto Array = generate_arrays<std::underlying_type_t<E>, BitFlagEnum<E>, Min, Max>();
    //	using EnumArray = std::array<E, Array.size()>;
    //	auto str = var_name<std::bit_cast<EnumArray>(Array)>();
    //	struct RetVal {
    //		std::array<Pair, Array.size()> pairs;
    //		std::size_t                 total_string_length = 0;
    //		std::size_t                 valid_count = 0;
    //	} ret;
    //	std::size_t    index = 0;
    //	constexpr auto enum_in_array_len = enum_in_array_name < E{} > ().size();
    //
    //	while (index < Array.size()) {
    //		if (str.front() == '(') {
    //			str.remove_prefix(sizeof("(") - 1 + length_of_enum_in_template_array_casting + sizeof(")0") - 1); // there is atleast 1 base 10 digit
    //			//if(!str.empty())
    //			//	std::cout << "after str \"" << str << '"' << '\n';
    //
    //			if (const auto commapos = str.find(",");commapos != str.npos)
    //				str.remove_prefix(commapos + 2);
    //
    //			//std::cout << "strsize \"" << str.size() << '"' << '\n';
    //		}
    //		else {
    //			str.remove_prefix(enum_in_array_len);
    //			if constexpr (enum_in_array_len != 0)
    //				str.remove_prefix(sizeof("::") - 1);
    //			const auto commapos = str.find(",");
    //
    //			const auto name = str.substr(0, commapos);
    //
    //			ret.pairs[index] = Pair{ E(Array[index]), name };
    //			ret.total_string_length += name.size() + 1;
    //
    //			if (commapos != str.npos)
    //				str.remove_prefix(commapos + 2);
    //			++ret.valid_count;
    //
    //
    //		}
    //		++index;
    //	}
    //	return ret;
    //	}();


    //std::cout << "ELEMET COUNT " << elements.pairs.size() << '\n';
    //for (auto& [e, v] : elements.pairs) {
    //	if(!v.empty())
    //	std::cout << '"' << v << '"' << '\n';
    //}

    //    [](const auto& pairs) {
    //  std::size_t count = 0;
    //  for (auto& [_, v] : pairs) {
    //    if (v.empty())
    //      continue;
    //
    //    ++count;
    //    count += v.size();
    //  }
    //  return count;
    //}(elements.pairs);


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
    //std::cout << "ELEMET COUNT " << elements.pairs.size() << '\n';

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

template<Enum E, typename Pair = std::pair<E, std::string_view>>
inline constexpr auto entries = details::reflect<E, Pair, enum_traits<E>::min, enum_traits<E>::max>();

} // namespace enchantum
