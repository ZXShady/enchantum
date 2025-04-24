#pragma once

#include "common.hpp"
#include <array>
#include <climits>
#include <cstdint>
#include <string_view>

#define SZC(x) (sizeof(x) - 1)


namespace enchantum {

namespace details {

  template<auto _>
  constexpr auto type_var_func() noexcept
  {
    /*
					constexpr auto enchantum::details::type_var_func() [with auto _ = (<unnamed>::LongNamespaced::Namespace2::<unnamed>::Really_Unreadable_Class_Name<int, long int, int***, TypeWithCommas<int, long int[3], TypeWithCommas<long int, int> > >::Numbers::A]
					*/
    /*
"constexpr auto enchantum::details::type_var_func() [with auto _ = TestEnum::False]"
"constexpr auto enchantum::details::type_var_func() [with auto _ = (TestEnum)0]"
*/
    constexpr auto len = SZC("constexpr auto enchantum::details::type_var_func()[with auto _ = ");
    if constexpr (__PRETTY_FUNCTION__[len + 1] == '(') // if it is a cast to an enum
    {
      constexpr auto s = std::string_view(__PRETTY_FUNCTION__ + len + SZC("(") + 1);
      return s.substr(0, s.rfind(')'));
    }
    else {
      constexpr auto s = std::string_view(__PRETTY_FUNCTION__ + len + SZC(" "));
      return s.substr(0, s.rfind("::"));
    }
  }

  template<typename _>
  constexpr auto type_name_func() noexcept
  {
    std::array<char, SZC(__PRETTY_FUNCTION__) - SZC("constexpr auto enchantum::details::type_name_func() [with _ = ]")> ret;
    for (std::size_t i = 0; i < ret.size(); ++i)
      ret[i] = __PRETTY_FUNCTION__[SZC("constexpr auto enchantum::details::type_name_func() [with _ = ") + i];
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
    int length = 0;
    do {
      ++length;
      number /= 10;
    } while (number > 0);
    return length;
  }


  constexpr std::int64_t totalBase10Digits(std::int64_t start, std::int64_t end)
  {
    std::int64_t total = 0;

    if (start < 0) {
      total += static_cast<std::int64_t>(-start) + totalBase10Digits(1, -start);
      start = 0;
    }

    for (std::int64_t i = start; i <= end;) {
      std::int64_t next_threshold = 10;

      while (next_threshold <= i)
        next_threshold *= 10;

      std::int64_t range_end = next_threshold - 1;
      range_end              = range_end < end ? range_end : end;

      int num_digits = numLength(i);
      total += (range_end - i + 1) * num_digits;

      i = range_end + 1;
    }

    return total;
  }


  constexpr std::size_t find_enum_name_and_skipped_count(const std::string_view funcsig,
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

  constexpr std::size_t count_substr(std::string_view s, std::string_view target) noexcept
  {
    std::size_t count = 0;
    std::size_t pos   = 0;
    while ((pos = s.find(target, pos)) != std::string_view::npos) {
      ++count;
      pos += target.length();
    }
    return count;
  }

  template<typename T>
  inline constexpr auto type_name_unnamed = type_var_func<T{}>();

  constexpr std::size_t find_enum_name(const std::string_view funcsig, const std::string_view enum_name)
  {
    std::size_t skipped = 0; // unused
    return find_enum_name_and_skipped_count(funcsig, enum_name, skipped);
  }

  // explicitly specifiying the calling convention so it is consistent in the __PRETTY_FUNCTION__
  template<auto V>
  constexpr auto var_name_func() noexcept
  {

    /*
					constexpr auto enchantum::details::var_name_func() [with auto V = std::array<{anonymous}::LongNamespaced::Namespace2::{anonymous}::Really_Unreadable_Class_Name2::Color, 2>{std::__array_traits<{anonymous}::LongNamespaced::Namespace2::{anonymous}::Really_Unreadable_Class_Name2::Color, 2>::_Type{(<unnamed>::LongNamespaced::Namespace2::<unnamed>::Really_Unreadable_Class_Name2::Color)-1, (<unnamed>::LongNamespaced::Namespace2::<unnamed>::Really_Unreadable_Class_Name2::Color)0}}]
					*/
    using T = typename decltype(V)::value_type;
#define SZC(x) (sizeof(x) - 1)
    /*
							constexpr auto enchantum::details::var_name_func() [with auto V = std::array<A, 6>{std::__array_traits<A, 6>::_Type{(A)-1, A::a, A::c, A::d, (A)4, (A)111}}]
							*/
    constexpr auto& type_name         = details::type_name<T>;
    constexpr auto& type_name_unnamed = details::type_name_unnamed<T>;
    // clang-format off
			constexpr auto prefix_len1 = SZC(
				"constexpr auto enchantum::details::var_name_func() [with auto V = std::array<")
				+ type_name.size()
				+ SZC(", ")
				+ numLength(V.size())
				+ SZC(">{std::__array_traits<")
				+ type_name.size()
				+ SZC(", ")
				+ numLength(V.size())
				+ SZC(">::_Type{"); // intentionally ommitted template params
			// clang-foramt on
			constexpr auto  type_repeat_count =
				(SZC("(), ") + type_name_unnamed.size()) * V.size() - SZC(", "); // for each element in array and twice in the declarations
			constexpr auto total_digit_length = totalBase10Digits(std::int64_t(V.front()), std::int64_t(V.back()));
			constexpr auto total = prefix_len1 + type_repeat_count + total_digit_length + SZC("}}]");
			//return __PRETTY_FUNCTION__;
			if constexpr (SZC(__PRETTY_FUNCTION__) == total)
				if constexpr (std::string_view::npos != details::find_enum_name(__PRETTY_FUNCTION__ + prefix_len1,
					std::string_view(type_name_unnamed.data(), type_name_unnamed.size())))
					return __PRETTY_FUNCTION__ + prefix_len1;
				else
					return "";
			else
				return __PRETTY_FUNCTION__ + prefix_len1;
			//return prefix_len1 + prefix_len2 + prefix_len3 + type_name_length * 2;
			//return std::pair(std::array<std::size_t,6>{prefix_len1,prefix_len2,prefix_len3,suffix_len1,type_repeat_count,total_digit_length},std::string_view(__PRETTY_FUNCTION__));
			//constexpr auto funcname = std::string_view(
			//  __PRETTY_FUNCTION__ + (sizeof("auto __cdecl enchantum::details::var_name_func<") - 1));
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
		inline constexpr auto var_name = var_name_func<V>();

		template<std::array V>
		constexpr auto reflection_data() noexcept
		{
			constexpr auto funcsig = std::string_view(details::var_name<V>);
			constexpr auto funcsig_len = funcsig.size();
			constexpr auto reflection_data = [name = funcsig]() mutable {
				constexpr const auto& enum_type_name_storage = type_name_unnamed<typename decltype(V)::value_type>;
				constexpr auto enum_type_name = std::string_view(enum_type_name_storage.data(), enum_type_name_storage.size());
				name.remove_suffix(sizeof("}}]") - 1);
				struct {
					char                       buffer[funcsig_len]{};
					std::array<bool, V.size()> enum_is_valid{};
					std::size_t                valid_count{};
				} ret_val;
				std::size_t buffer_index = 0;

				std::size_t skipped = 0;
				auto        pos = find_enum_name_and_skipped_count(name, enum_type_name, skipped);
				while (pos != name.npos) {
					if (const auto comma = name.find(",", pos); comma != name.npos) {
						const auto enum_name = name.substr(pos, comma - pos);
						for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
							ret_val.buffer[buffer_index + i] = enum_name[i];

						buffer_index += enum_name.size();
						buffer_index += 1;
						name.remove_prefix(comma + 1); // one past comma
					}
					else {
						// Text is
						// A::Val
						auto enum_name = name.substr(pos);
						for (std::size_t i = 0, size = enum_name.size(); i < size; ++i)
							ret_val.buffer[buffer_index + i] = enum_name[i];
					
						buffer_index += enum_name.size();
						name = std::string_view{}; // end of string
					}
					++ret_val.valid_count;
					ret_val.enum_is_valid[skipped] = true;
					pos = find_enum_name_and_skipped_count(name, enum_type_name, skipped); 
					// The string is shortened by `.remove_prefix` therefore we need to account for the element passed
					++skipped; 
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
				data.is_valid = reflection_data.enum_is_valid;
				return data;
				}();
		}

		template<auto V>
		inline constexpr auto static_var = reflection_data<V>();

		template<typename Enum, auto Min, auto Max>
		constexpr auto array_of_enums()
		{
			if constexpr (BitFlagEnum<Enum>) {
				using T = std::underlying_type_t<Enum>;
				constexpr std::size_t                 bits = sizeof(T) * CHAR_BIT;
				std::array<std::array<Enum, bits>, 1> a{};
				for (std::size_t i = 0; i < bits; ++i)
					a[0][i] = static_cast<Enum>(static_cast<std::make_unsigned_t<T>>(1) << i);
				return a;
			}
			else {
				static_assert(Min < Max, "Min must be less than Max");

				constexpr auto        ChunkSize = ENCHANTUM_SEARCH_PER_ITERATION;
				constexpr std::size_t total = Max - Min;
				constexpr std::size_t chunks = (total + ChunkSize - 1) / ChunkSize; // ceiling division

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
			using T = typename Array::value_type;
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
			return[]<auto data>() {
				if constexpr (false || data.valid_count == 0) {
					return std::array<Pair, 0>{};
				}
				else {
					std::array<Pair, data.valid_count> ret;
					for (std::size_t i = 0, count = 0, ret_index = 0; i < sizeof(data.strings); ++i) {
						if (data.strings[i] == '\0') {
							//const auto& da    = data; // for debugging
							auto& [_, string] = ret[ret_index];
							string = std::string_view(data.strings + count, i - count);
							count = i + 1;
							++ret_index;
						}
					}

					for (std::size_t i = 0, ret_index = 0; i < data.is_valid.size(); ++i) {
						if (data.is_valid[i]) {
							auto& [val, _] = ret[ret_index];
							val = array_of_enums[i];
							++ret_index;
						}
					}
					return ret;
				}
			}.template operator() < data > ();
		}
	}

	template<Enum E, typename Pair = std::pair<E, std::string_view>>
	inline constexpr auto entries = []() {
		using traits = enum_traits<E>;
		//#if 0
		constexpr auto array_of_array_enums = details::array_of_enums<E, traits::min, traits::max>(); // not to have to account for stupid trailing element
		//  #else
		//  constexpr auto array_of_array_enums_ = details::array_of_enums<std::underlying_type_t<E>, traits::min, traits::max>();
		//  constexpr auto array_of_array_enums  = std::bit_cast <
		//    std::array<std::array<Enum, ENCHANTUM_SEARCH_PER_ITERATION>, array_of_array_enums_.size()>>(array_of_array_enums_);
		//#endif
		return[]<std::array A, std::size_t... Idx>(std::index_sequence<Idx...>) {
			return enchantum::details::concat_arrays(enchantum::reflect<A[Idx], Pair>()...);
		}.template operator() < array_of_array_enums > (std::make_index_sequence<array_of_array_enums.size()>());
		//  return details::entries_iteration<a, E, Pair>();
		}();

	template<EnumOfUnderlying<bool> E, typename Pair>
	inline constexpr auto entries<E, Pair> = enchantum::reflect < std::array{ E(false), E(true) }, Pair > (); // apparently this works???

} // namespace enchantum



#undef SZC