#include <enchantum/enchantum.hpp>

enum class A : int;

int main() 
{
    static_cast<void>(enchantum::count<A>);
}

