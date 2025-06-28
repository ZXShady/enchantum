#pragma once
#include "entries.hpp"
#include <compare>
#include <cstddef>
#include <cstdint>

namespace enchantum {
namespace details {


  struct senitiel {};

  template<typename CRTP,std::size_t Size>
  struct sized_iterator {
  private:
    using IndexType = std::conditional_t<(Size < INT16_MAX), std::int8_t, std::int16_t>;
  public:
    IndexType                 index{};
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

    constexpr CRTP operator++(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      ++*this;
      return copy;
    }
    constexpr sized_iterator& operator--(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      --*this;
      return copy;
    }

    constexpr friend CRTP operator+(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it += offset;
      return it;
    }

    constexpr friend CRTP operator+(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it += offset;
      return it;
    }

    constexpr friend CRTP operator-(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it -= offset;
      return it;
    }

    constexpr friend CRTP operator-(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it -= offset;
      return it;
    }

    constexpr friend std::ptrdiff_t operator-(const CRTP a, const CRTP b) noexcept
    {
      return a.index - b.index;
    }

    constexpr bool operator==(const CRTP that) const noexcept { return that.index == index; };
    constexpr auto operator<=>(const CRTP that) const noexcept { return index <=> that.index; };
    constexpr bool operator==(senitiel) const noexcept { return Size == index; }
    constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
  };

#define SZARR(x) (sizeof(x) / sizeof(x[0]))
  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");


    struct iterator : sized_iterator<iterator,size()> {
      constexpr String operator*() const noexcept
      {
        const auto* const p       = reflection_string_indices<E, NullTerminated>.data();
        const auto* const strings = reflection_data<E, NullTerminated>.strings;
        return String(strings + p[this->index], strings + p[this->index + 1] - NullTerminated);
      }

      constexpr String operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    static constexpr auto begin() { return iterator{}; }
    static constexpr auto end() { return senitiel{}; }

    constexpr auto operator[](const std::size_t i) const noexcept {return *(begin()+static_cast<std::ptrdiff_t>(i)); }
  };

  template<typename E>
  struct values_generator_t {
    static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator,size()> {
    public:
      constexpr E operator*() const noexcept
      {
        if constexpr (is_contiguous<E>)
          return static_cast<E>(enchantum::to_underlying(min<E>) + static_cast<std::underlying_type_t<E>>(this->index));
        else
          return values<E>[static_cast<std::size_t>(this->index)];
      }
      constexpr E operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    static constexpr auto begin() { return iterator{}; }
    static constexpr auto end() { return senitiel{}; }

    constexpr auto operator[](const std::size_t i) const noexcept { return *(begin() + static_cast<std::ptrdiff_t>(i)); }
  };

  template<typename E, typename Pair = std::pair<E, string_view>,bool NullTerminated=true>
  struct entries_generator_t {
    static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator,size()> {
    public:
      constexpr Pair operator*() const noexcept
      {
        return Pair{
          values_generator_t<E>{}[static_cast<std::size_t>(this->index)],
          names_generator_t<E,string_view,NullTerminated>{}[static_cast<std::size_t>(this->index)],
        };
      }
      constexpr Pair operator[](const std::ptrdiff_t i) const noexcept { 
          return *(*this+i); }
    };

    static constexpr auto begin() { return iterator{}; }
    static constexpr auto end() { return senitiel{}; }

    constexpr auto operator[](const std::size_t i) const noexcept { return *(begin()+static_cast<std::ptrdiff_t>(i)); }
  };

} // namespace details

template<Enum E,typename StringView = string_view,bool NullTerminated=true>
inline constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E>
inline constexpr details::values_generator_t<E> values_generator{};

template<Enum E, typename Pair = std::pair<E,string_view>, bool NullTerminated = true>
inline constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};


} // namespace enchantum