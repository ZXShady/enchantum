#pragma once
#include "entries.hpp"
#ifdef __cpp_impl_three_way_comparison
  #include <compare>
#endif
#include "details/countr_zero.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  // false positives from T += T
  // it does not make sense.
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace enchantum {
namespace details {

  struct senitiel {};


  template<typename CRTP, std::ptrdiff_t Size>
  struct sized_iterator {
    static_assert(Size < INT16_MAX, "Too many enum entries");
  private:
    using IndexType = std::conditional_t<(Size <= INT8_MAX), std::int8_t, std::int16_t>;
  public:
    IndexType       index{};
    constexpr CRTP& operator+=(const std::ptrdiff_t offset) & noexcept
    {
      index += static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator-=(const std::ptrdiff_t offset) & noexcept
    {
      index -= static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }

    constexpr CRTP& operator++() & noexcept
    {
      ++index;
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator--() & noexcept
    {
      --index;
      return static_cast<CRTP&>(*this);
    }

    [[nodiscard]] constexpr CRTP operator++(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      ++*this;
      return copy;
    }
    [[nodiscard]] constexpr CRTP operator--(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      --*this;
      return copy;
    }

    [[nodiscard]] constexpr friend CRTP operator+(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it += offset;
      return it;
    }

    [[nodiscard]] constexpr friend CRTP operator+(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it += offset;
      return it;
    }

    [[nodiscard]] constexpr friend CRTP operator-(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it -= offset;
      return it;
    }

    [[nodiscard]] constexpr std::ptrdiff_t operator-(const sized_iterator that) const noexcept
    {
      return index - that.index;
    }

    [[nodiscard]] constexpr std::ptrdiff_t        operator-(senitiel) const noexcept { return index - Size; }
    [[nodiscard]] friend constexpr std::ptrdiff_t operator-(senitiel, sized_iterator it) noexcept
    {
      return Size - it.index;
    }

    [[nodiscard]] constexpr bool operator==(const sized_iterator that) const noexcept { return that.index == index; };
    [[nodiscard]] constexpr bool operator==(senitiel) const noexcept { return Size == index; }

#ifdef __cpp_impl_three_way_comparison
    [[nodiscard]] constexpr auto operator<=>(const sized_iterator that) const noexcept { return index <=> that.index; };
    [[nodiscard]] constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
#else

    [[nodiscard]] constexpr bool operator!=(const sized_iterator that) const noexcept { return that.index != index; };
    [[nodiscard]] constexpr bool operator!=(senitiel) const noexcept { return Size != index; }

    [[nodiscard]] friend constexpr bool operator==(senitiel, const sized_iterator it) noexcept
    {
      return Size == it.index;
    }


    [[nodiscard]] friend constexpr bool operator!=(senitiel, const sized_iterator it) noexcept
    {
      return Size != it.index;
    }


    [[nodiscard]] constexpr bool operator<(const sized_iterator that) const noexcept { return index < that.index; };
    [[nodiscard]] constexpr bool operator>(const sized_iterator that) const noexcept { return index > that.index; };
    [[nodiscard]] constexpr bool operator<=(const sized_iterator that) const noexcept { return index <= that.index; };
    [[nodiscard]] constexpr bool operator>=(const sized_iterator that) const noexcept { return index >= that.index; };

    [[nodiscard]] constexpr bool operator<(senitiel) const noexcept { return index < Size; };
    [[nodiscard]] constexpr bool operator>(senitiel) const noexcept { return index > Size; };
    [[nodiscard]] constexpr bool operator<=(senitiel) const noexcept { return index <= Size; };
    [[nodiscard]] constexpr bool operator>=(senitiel) const noexcept { return index >= Size; };

    [[nodiscard]] friend constexpr bool operator<(senitiel, const sized_iterator it) noexcept
    {
      return Size < it.index;
    };
    [[nodiscard]] friend constexpr bool operator>(senitiel, const sized_iterator it) noexcept
    {
      return Size > it.index;
    };
    [[nodiscard]] friend constexpr bool operator<=(senitiel, const sized_iterator it) noexcept
    {
      return Size <= it.index;
    };
    [[nodiscard]] friend constexpr bool operator>=(senitiel, const sized_iterator it) noexcept
    {
      return Size >= it.index;
    };

#endif
  };

  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = String;
      [[nodiscard]] constexpr String operator*() const noexcept
      {
        const auto* const p       = details::reflection_string_indices<E, NullTerminated>.data();
        const auto* const strings = details::reflection_data_string_storage<E, NullTerminated>.data();
        return String(strings + p[this->index], p[this->index + 1] - p[this->index] - NullTerminated);
      }

      [[nodiscard]] constexpr String operator[](const std::ptrdiff_t i) const noexcept
      {
        auto it = *this;
        it += i;
        return *it;
      }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      auto it = begin();
      it += static_cast<std::ptrdiff_t>(i);
      return *it;
    }
  };

  template<typename E>
  struct values_generator_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = E;
      [[nodiscard]] constexpr E dereference(std::true_type, std::false_type) const noexcept
      {
        using T = typename std::underlying_type<E>::type;
        return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
      }

      [[nodiscard]] constexpr E dereference(std::false_type, std::true_type) const noexcept
      {
        using T                        = typename std::underlying_type<E>::type;
        using UT                       = typename std::make_unsigned<T>::type;
        constexpr auto real_min_offset = details::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));
        if (has_zero_flag<E> ? this->index == 0 : false)
          return E{};
        return static_cast<E>(UT{1} << (real_min_offset + static_cast<UT>(this->index - has_zero_flag<E>)));
      }

      [[nodiscard]] constexpr E dereference(std::false_type, std::false_type) const noexcept
      {
        return values<E>[static_cast<std::size_t>(this->index)];
      }

      [[nodiscard]] constexpr E operator*() const noexcept
      {
        return dereference(std::integral_constant<bool, is_contiguous<E>>{},
                           std::integral_constant<bool, is_contiguous_bitflag<E>>{});
      }
      [[nodiscard]] constexpr E operator[](const std::ptrdiff_t i) const noexcept
      {
        auto it = *this;
        it += i;
        return *it;
      }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      auto it = begin();
      it += static_cast<std::ptrdiff_t>(i);
      return *it;
    }
  };

  template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
  struct entries_generator_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = Pair;
      [[nodiscard]] constexpr Pair operator*() const noexcept
      {
        return Pair{
          values_generator_t<E>{}[static_cast<std::size_t>(this->index)],
          names_generator_t<E, string_view, NullTerminated>{}[static_cast<std::size_t>(this->index)],
        };
      }
      [[nodiscard]] constexpr Pair operator[](const std::ptrdiff_t i) const noexcept
      {
        auto it = *this;
        it += i;
        return *it;
      }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      auto it = begin();
      it += static_cast<std::ptrdiff_t>(i);
      return *it;
    }
  };

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::values_generator_t<E> values_generator{};

#ifdef __cpp_concepts
template<Enum E, typename StringView = string_view, bool NullTerminated = true>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#else
template<typename E, typename StringView = string_view, bool NullTerminated = true, std::enable_if_t<std::is_enum<E>::value, int> = 0>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true, std::enable_if_t<std::is_enum<E>::value, int> = 0>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#endif

} // namespace enchantum

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
