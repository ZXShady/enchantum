#include "../common.hpp"
#include "../type_name.hpp"
#include "generate_arrays.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "string_view.hpp"

namespace enchantum {
	namespace details {
#define SZC(x) (sizeof(x) - 1)
		// this is needed since gcc transforms "{anonymous}" into "<unnamed>" for values
		template<auto Enum>
		constexpr auto enum_in_array_name_size() noexcept
		{
			// constexpr auto f() [with auto _ = (
			//constexpr auto f() [with auto _ = (Scoped)0]
			auto s = string_view(__PRETTY_FUNCTION__ +
				SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with auto Enum = "),
				SZC(__PRETTY_FUNCTION__) -
				SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with auto Enum = ]"));
			using E = decltype(Enum);
			// if scoped
			if constexpr (!std::is_convertible_v<E, std::underlying_type_t<E>>) {
				return s.front() == '(' ? s.size() - SZC("()0") : s.rfind(':') - 1;
			}
			else {
				if (s.front() == '(') {
					s.remove_prefix(SZC("("));
					s.remove_suffix(SZC(")0"));
				}
				if (const auto pos = s.rfind(':'); pos != s.npos)
					return pos - 1;
				return std::size_t{ 0 };
			}
		}

		template<typename Enum>
		constexpr auto length_of_enum_in_template_array_if_casting() noexcept
		{
			if constexpr (ScopedEnum<Enum>) {
				return details::enum_in_array_name_size < Enum{} > ();
			}
			else {
				constexpr auto  s = details::enum_in_array_name_size < Enum{} > ();
				constexpr auto& tyname = raw_type_name<Enum>;
				if (constexpr auto pos = tyname.rfind("::"); pos != tyname.npos) {
					return s + tyname.substr(pos).size();
				}
				else {
					return s + tyname.size();
				}
			}
		}

		template<auto... Vs>
		constexpr auto var_name() noexcept
		{
			//constexpr auto f() [with auto _ = std::array<E, 6>{std::__array_traits<E, 6>::_Type{a, b, c, e, d, (E)6}}]
			constexpr std::size_t funcsig_off = SZC("constexpr auto enchantum::details::var_name() [with auto ...Vs = {");
			return std::string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - SZC("}]"));
		}


		template<auto Copy>
		inline constexpr auto static_storage_for = Copy;


		template<typename Underlying, std::size_t ArraySize, std::size_t MaxStringLength>
		struct ReflectStringReturnValue {
			Underlying            values[ArraySize];
			std::uint8_t string_lengths[ArraySize];
			char         strings[MaxStringLength]{};
			std::size_t  total_string_length = 0;
			std::size_t  valid_count = 0;
		};



		template<bool IsBitFlag, typename IntType>
		constexpr void parse_string(const char* str, const std::size_t least_length_when_casting, const std::size_t least_length_when_value, const IntType min,
			const std::size_t array_size,
			const bool null_terminated,
			IntType* const values,
			std::uint8_t* const string_lengths,
			char* const strings,
			std::size_t& total_string_length,
			std::size_t& valid_count
		)
		{
			for (std::size_t index = 0; index < array_size; ++index) {
				
				if (*str == '(') {
					str += least_length_when_casting;
					str += static_cast<std::size_t>(std::char_traits<char>::find(str, UINT8_MAX, ',') - str) + SZC(", ");
				}
				else {
					str += least_length_when_value;
					// although gcc implementation of std::char_traits::find is using a for loop internally
					// copying the code of the function makes it way slower to compile, this was surprising.
					const auto commapos = static_cast<std::size_t>(std::char_traits<char>::find(str, UINT8_MAX, ',') - str);
					const auto name_size = static_cast<std::uint8_t>(commapos);
					if constexpr (IsBitFlag)
						values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{ 1 } << (index - 1));
					else
						values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
					string_lengths[valid_count++] = name_size;
					for (std::size_t i = 0; i < name_size; ++i)
						strings[total_string_length++] = str[i];
					total_string_length += null_terminated;
					str += commapos + SZC(", ");
				}
			}
		}

		template<typename E, bool NullTerminated>
		constexpr auto reflect() noexcept
		{
			constexpr auto Min = enum_traits<E>::min;
			constexpr auto Max = enum_traits<E>::max;

			constexpr auto elements = []() {
				constexpr auto ArraySize = 1 + std::size_t{ is_bitflag<E> ? (sizeof(E) * CHAR_BIT - std::is_signed_v<E>) : Max - Min };
				using Under = std::underlying_type_t<E>;
				using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;


				constexpr auto ConstStr = []<std::size_t... Idx>(std::index_sequence<Idx...>) {
					// __builtin_bit_cast used to silence errors when casting out of unscoped enums range
					// dummy 0
					if constexpr (sizeof...(Idx) && is_bitflag<E>) // sizeof... to make contest dependant
						return details::var_name < E{}, __builtin_bit_cast(E, static_cast<Under>(Underlying(1) << Idx))..., 0 > ();
					else
						return details::var_name<__builtin_bit_cast(E, static_cast<Under>(static_cast<decltype(Min)>(Idx) + Min))..., 0>();
				}(std::make_index_sequence<ArraySize - is_bitflag<E>>());

				constexpr auto enum_in_array_len = details::enum_in_array_name_size < E{} > ();
				constexpr auto length_of_enum_in_template_array_casting = details::length_of_enum_in_template_array_if_casting<E>();

				ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize, ConstStr.size()> ret;
				details::parse_string<is_bitflag<E>>(
					/*str = */ ConstStr.data(),
					/*least_length_when_casting=*/ SZC("(") + length_of_enum_in_template_array_casting + SZC(")0"),
					/*least_length_when_value=*/  details::prefix_length_or_zero<E> +(enum_in_array_len != 0 ? enum_in_array_len + SZC("::") : 0),
					/*min = */ static_cast<std::underlying_type_t<E>>(Min),
					/*array_size = */  ArraySize,
					/*null_terminated= */ NullTerminated,
					/*enum_values= */ ret.values,
					/*string_lengths= */ ret.string_lengths,
					/*strings= */ ret.strings,
					/*total_string_length*/ ret.total_string_length,
					/*valid_count*/ ret.valid_count
				);
				return ret;
				}();

			constexpr auto strings = [](const auto total_length, const char* const data) {
				std::array<char, total_length.value> ret;
				auto* const                          ret_data = ret.data();
				for (std::size_t i = 0; i < total_length.value; ++i)
					ret_data[i] = data[i];
				return ret;
				}(std::integral_constant<std::size_t, elements.total_string_length>{}, elements.strings);

			using StringLengthType = std::conditional_t<(elements.total_string_length < UINT8_MAX), std::uint8_t, std::uint16_t>;

			struct RetVal {
				std::array<E, elements.valid_count> values{};
				// +1 for easier iteration on on last string
				std::array<StringLengthType, elements.valid_count + 1> string_indices{};
				const char* strings{};
			} ret;
			ret.strings = static_storage_for<strings>.data();

			std::size_t      i = 0;
			StringLengthType string_index = 0;
			for (; i < elements.valid_count; ++i) {
				ret.values[i] = static_cast<E>(elements.values[i]);
				// "aabc"

				ret.string_indices[i] = string_index;
				string_index += static_cast<StringLengthType>(elements.string_lengths[i] + NullTerminated);
			}
			ret.string_indices[i] = string_index;
			return ret;
		}

	} // namespace details

} // namespace enchantum

#undef SZC