#pragma once
#include <nlohmann/adl_serializer.hpp>
#include "enchantum.hpp"
#include "bitflags.hpp"


#ifdef __cpp_concepts
template<enchantum::Enum E>
struct nlohmann::adl_serializer<E>
#else
template<typename E>
struct nlohmann::adl_serializer<E,std::enable_if_t<std::is_enum_v<E>>>
#endif
{
    template<typename J>
    static E from_json(const J& j)
    {
        using type_error = typename J::type_error;
        if(const auto* s = j.template get_ptr<const typename J::string_t*>()) {
            if constexpr(enchantum::is_bitflag<E>) {
                if(const auto e = enchantum::cast_bitflag<E>(*s))
                    return *e;
            } else {
                if(const auto e = enchantum::cast<E>(*s))
                    return *e;
            }
            ENCHANTUM_THROW(type_error::create(302, "invalid enum value.", &j),j);
        }
        ENCHANTUM_THROW(type_error::create(302, "expected string for an enum.", &j),j);
    }
    template<typename J>
    static void to_json(J& j,E e)
    {
        using out_of_range = typename J::out_of_range;
        if constexpr(enchantum::is_bitflag<E>)
        {
            const auto s = enchantum::to_string_bitflag(e);
            if(s.empty())
                ENCHANTUM_THROW(out_of_range::create(302, "invalid input enum value.", &j),e);
            j = s;
        } else {
            const auto s = enchantum::to_string(e);
            if(s.empty())
                ENCHANTUM_THROW(out_of_range::create(302, "invalid input enum value.", &j),e);
            j = s;
        }
    }
};
