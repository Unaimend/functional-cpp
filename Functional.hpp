#ifndef __FUNCTIONAL__HPP__
#define __FUNCTIONAL__HPP__
#include <vector>
#include <functional>


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
    template <typename T>
    std::vector<T> map(const std::vector<T>& vector, std::function<T(const T)> f)
    {
        // Performs copy ellision which is required by standard since c++17
        //http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4713.pdf#page=258
        std::vector<T> temp;

        for(const T& it: vector)
        {
            temp.push_back(f(it));
        }
    return temp;
    }

    template <typename C, typename T>
    T fold(const C& container, std::function<T(const T, const T)> f, const T accu)
    {
        T accumulator = accu;
        for(auto it = container.begin(); it != container.end(); ++it)
        {
            accumulator = f(*it, accumulator);
        }
        return accumulator;
    }

    template< typename T>
    T sum(const std::vector<T>& vector)
    {
        return fold<std::vector<int>, int>(vector,[](const int a, const int b){return a + b;}, 0);
    }


}
}
#endif

