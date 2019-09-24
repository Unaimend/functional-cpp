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

/*
template <typename C,
typename T = typename C::value_type>
std::unordered_map<T, unsigned int> count_occurrences(
const C& collection)
*/


    //TODO Genauer ueber die performance probleme von std::function informieren
namespace pure
{
    // Statt der template spezialisierungen sollte ich concepts 
    template<class A, class B, class C, class F>
    auto flip( F func) -> std::function<A(C, B)>
    {
        static_assert(
            std::is_convertible<F,std::function<A(B, C)>>::value,
            "flip requires a function of A (B,C)"
            );
        using namespace std::placeholders;
        return std::bind(func, _2, _1);
    }

    template <typename C, typename T, typename F>
    C map(const C& vector, F f)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T)>>::value,
            "map requires a function of T (T)" //TODO Tut es nicht T1 (T2) waere korrekt
            );

        C temp;
        for(const T& it: vector)
        {
            temp.push_back(f(it));
        }
        return temp;
    }


    template <typename T, typename F>
    std::list<T> map(const std::list<T> list,  F f)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T)>>::value,
            "map requires a function of T (T)"
            );
        std::list<T> temp;

        for(const T& it: list)
        {
            temp.push_back(f(it));
        }
        return temp;
    }

    // Ist es unnoetig F in den funktionsparametern mitzugeben(siehe sum. bzgl. std::plus<T>)
    template <typename C, typename T, typename F>
    T fold(const C& container, F f, const T accu)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T, const T)>>::value,
            "fold requires a function of T (T, T)"
            );

        T accumulator = accu;
        for(const auto& it: container)
        {
            accumulator = f(it, accumulator);
        }
        return accumulator;
    }

    template <typename T, typename C, typename F>
    C filter(const C& container, F f)
    {
        // TODO Das muss schoener gehen
        T val;
        using ret_type = decltype(f(val));
        static_assert(std::is_same<ret_type, bool>::value);
        C temp;
        for(const auto& it : container)
        {
            if(f(it))
            {
                temp.push_back(it);
            }
        }
        return temp;
    }


    template <typename T, typename F>
    T fold(std::list<T> list, F f, const T accu)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T, const T)>>::value,
            "fold requires a function of T (T, T)"
            );
        T accumulator = accu;
        for(const auto& it: list)
        {
            accumulator = f(it, accumulator);
        }
        return accumulator;
    }

    // TOTHINK replace std::bind with lambdas so the compiler cant optimize more easily
    template<typename C, typename T>
    std::function<T(const C& container)> sum = std::bind(fold<C, T, std::plus<T>>,std::placeholders::_1, std::plus<T>(),0);

    template<typename C, typename T>
    std::function<T(const C& container)> product = std::bind(fold<C, T, std::multiplies<T>>,std::placeholders::_1, std::multiplies<T>(),1);
}
}
#endif

