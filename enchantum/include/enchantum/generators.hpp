#pragma once
#include "entries.hpp"
#include <bit>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace enchantum {
namespace details {


  struct senitiel {};

  template<typename CRTP, std::size_t Size>
  struct sized_iterator {
  private:
    using IndexType = std::conditional_t<(Size < INT16_MAX), std::int8_t, std::int16_t>;
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

    [[nodiscard]] constexpr friend CRTP operator-(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it -= offset;
      return it;
    }

    [[nodiscard]] constexpr friend std::ptrdiff_t operator-(const CRTP a, const CRTP b) noexcept
    {
      return a.index - b.index;
    }

    [[nodiscard]] constexpr bool operator==(const CRTP that) const noexcept { return that.index == index; };
    [[nodiscard]] constexpr auto operator<=>(const CRTP that) const noexcept { return index <=> that.index; };
    [[nodiscard]] constexpr bool operator==(senitiel) const noexcept { return Size == index; }
    [[nodiscard]] constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
  };

  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    [[nodiscard]] static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");


    struct iterator : sized_iterator<iterator, size()> {
      [[nodiscard]] constexpr String operator*() const noexcept
      {
        // Access the raw reflected data through static_storage in details
        constexpr auto& reflected = details::storage_v<E, NullTerminated>.reflected_raw;
        const auto* const p       = reflected.string_indices.data(); // Or just reflected.string_indices
        const auto* const strings = reflected.strings;
        return String(strings + p[this->index], strings + p[this->index + 1] - NullTerminated);
      }

      [[nodiscard]] constexpr String operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

  template<typename E>
  struct values_generator_t {
    [[nodiscard]] static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator, size()> {
    public:
      [[nodiscard]] constexpr E operator*() const noexcept
      {
        using T = std::underlying_type_t<E>;

        if constexpr (is_contiguous<E>) {
          return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
        }
        else if constexpr (is_contiguous_bitflag<E>) {
          using UT                       = std::make_unsigned_t<T>;
          constexpr auto real_min_offset = std::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));

          if constexpr (has_zero_flag<E>)
            if (this->index == 0)
              return E{};
          return static_cast<E>(UT{1} << (real_min_offset + static_cast<UT>(this->index - has_zero_flag<E>)));
        }
        else {
          return values<E>[static_cast<std::size_t>(this->index)];
        }
      }
      [[nodiscard]] constexpr E operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

  template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
  struct entries_generator_t {
    [[nodiscard]] static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator, size()> {
    public:
      [[nodiscard]] constexpr Pair operator*() const noexcept
      {
        return Pair{
          values_generator_t<E>{}[static_cast<std::size_t>(this->index)],
          names_generator_t<E, string_view, NullTerminated>{}[static_cast<std::size_t>(this->index)],
        };
      }
      [[nodiscard]] constexpr Pair operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

} // namespace details

template<Enum E, typename StringView = string_view, bool NullTerminated = true>
inline constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E>
inline constexpr details::values_generator_t<E> values_generator{};

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
inline constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};


} // namespace enchantum