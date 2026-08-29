#include <enchantum/all.hpp>

enum E {a,b,c,d,e};
int main()
{
    static_assert(enchantum::count<E> == 5);
}