#ifndef __FUNCTIONAL__HPP__
#define __FUNCTIONAL__HPP__
#include <list>
#include <vector>
#include <functional>
#include <type_traits>


namespace func
{
namespace inplace
{
    template <typename F, typename T>
    void map(std::vector<T>& vector, F f)
    {
        for(T& it: vector)
        {
            f(it);
        }
    }
}

namespace pure
{
    // STATT std::fuction<...>, F f und std::is_convetible_to verwenden
    template<class A, class B, class C>
    std::function<A(C, B)> flip(std::function<A(B, C)> func)
    {
        using namespace std::placeholders;
        return std::bind(func, _2, _1);
    }

    template <typename C, typename T>
    C map(const C& vector, std::function<T(const T)> f)
    {
        // Performs copy ellision which is required by standard since c++17
        //http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4713.pdf#page=258
        C temp;

        for(const T& it: vector)
        {
            temp.push_back(f(it));
        }
    return temp;
    }

    template <typename C, typename T, typename F>
    T fold(const C& container, F f, const T accu)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T, const T)>>::value,
            "fold requires a function of T <>(T, T)"
            );

        std::cout << "CALLED2 " << std::endl;
        T accumulator = accu;
        for(const auto& it: container)
        {
            accumulator = f(it, accumulator);
        }
        return accumulator;
    }


    template <typename T, typename F>
    T fold(std::list<T> list,  F f, const T accu)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T, const T)>>::value,
            "fold requires a function of T <>(T, T)"
            );
        std::cout << "CALLED " << std::endl;
        T accumulator = accu;
        for(const auto& it: list)
        {
            accumulator = f(it, accumulator);
        }
        return accumulator;
    }

    /*template<typename C, typename T>
    std::function<T(const C& container)> sum = std::bind(fold<C, T>,std::placeholders::_1, std::plus<T>(),0);
    */

}
}
#endif

