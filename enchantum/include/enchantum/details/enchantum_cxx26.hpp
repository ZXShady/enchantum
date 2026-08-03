#include "../common.hpp"
#include "shared.hpp"
#include "string_view.hpp"
#include <meta>
#include <type_traits>

namespace enchantum {
namespace details {
  template<typename E>
  constexpr std::size_t sort_unique(std::meta::info* data, std::size_t size)
  {
    using T = std::underlying_type_t<E>;
    for (std::size_t i = 0; i < size; ++i) {
      for (std::size_t j = 0; j + 1 < size - i; ++j) {
        if (static_cast<T>(extract<E>(data[j])) > static_cast<T>(extract<E>(data[j + 1]))) {
          const auto t = data[j];
          data[j]      = data[j + 1];
          data[j + 1]  = t;
        }
      }
    }

    if (size == 0)
      return 0;

    std::size_t newsize = 1;
    for (std::size_t i = 1; i < size; ++i)
      if (static_cast<T>(extract<E>(data[i])) != static_cast<T>(extract<E>(data[newsize - 1])))
        data[newsize++] = data[i];

    return newsize;
  }

  template<typename E, bool NullTerminated, auto>
  constexpr auto reflect(...) noexcept
  {
    constexpr auto elements_local = []() {
      using T            = std::underlying_type_t<E>;
      auto       r       = std::meta::enumerators_of(^^E);
      const auto newsize = details::sort_unique<E>(r.data(), r.size());

      ReflectStringReturnValue<T, std::meta::enumerators_of(^^E).size()> ret;
      char*                                                              p = ret.strings;
      for (size_t i = 0; i < newsize; ++i) {
        const auto m          = r[i];
        const auto n          = std::meta::identifier_of(m);
        ret.string_lengths[i] = static_cast<std::uint8_t>(n.size());
        for (const auto c : n)
          *p++ = c;
        p += NullTerminated;

        ret.total_string_length += n.size();
        ret.values[i] = static_cast<T>(std::meta::extract<E>(m));
      }
      ret.valid_count = r.size();
      ret.total_string_length += (NullTerminated ? r.size() : 0);
      return ret;
    }();

    using Strings = std::array<char, elements_local.total_string_length>;
    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data                  = {elements_local};
    const auto  size        = data.strings.size();
    auto* const data_string = data.strings.data();
    for (std::size_t i = 0; i < size; ++i)
      data_string[i] = elements_local.strings[i];
    return data;
  }

  template<typename, auto>
  constexpr bool is_out_of_range(...) noexcept
  {
    return false;
  }

} // namespace details

} // namespace enchantum
