#define ENCHANTUM_MIN_RANGE 0
#define ENCHANTUM_MAX_RANGE 50
#include <enchantum/enchantum.hpp>

enum class A { value = 30,a = 70 };
int main() 
{
#if defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L
    static_assert(false,"to pass the test");
#else
    static_cast<void>(enchantum::count<A>);
#endif
}

