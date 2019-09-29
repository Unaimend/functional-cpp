#ifndef __FUNCTIONAL__HPP__
#define __FUNCTIONAL__HPP__
#include <list>
#include <vector>
#include <functional>
#include <type_traits>
#include <mutex>
#include <memory>
#include <optional>
#include <cstddef>
#include <stdexcept>
#include <cassert>
#include <iostream>
//TODO Read https://www.fluentcpp.com/2018/04/24/following-conventions-stl/
//TODO https://hannes.hauswedell.net/post/2018/04/11/view1/


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


    //TODO Use ContainerIn and ContainerOut instead of nested templates
    //https://github.com/Dobiasd/FunctionalPlus/blob/master/include/fplus/transform.hpp
    template <typename ArgType,
              typename F,
              typename Ret = typename std::result_of<F(ArgType)>::type,
              template <typename> typename Container>
    auto map(const Container<ArgType>& vector, F f)
    {

        static_assert(
            std::is_convertible<F,std::function<Ret(const ArgType)>>::value,
            "map requires a function of const Ret (const ArgType)" 
            );

        static_assert(
            std::is_copy_constructible<Container<ArgType>>::value,
            "The container must be copy constructible"
            );

        static_assert(
            std::is_move_assignable<Container<ArgType>>::value,
            "The container must be move assinable"
            );

        Container<Ret> temp;
        //Container must support iteration
        for(const ArgType& it: vector)
        {
            temp.push_back(f(it));
            // temp.push_back(f(it));
        }
        //Cotainer must be copyable
        return temp;
    }

    // Ist es unnoetig F in den funktionsparametern mitzugeben(siehe sum. bzgl. std::plus<T>)
    template <typename C,
              typename T = typename C::value_type,
              typename F>
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




    //TODO Implement optimizations for rvalues(eliminating copies)
    //push_back() && push_front() &&
    template<typename T>
    class List
    {
        template<typename D>
        class Node
        {
        public:
            Node(T val) : value(val){}
            T value;
            std::shared_ptr<Node<T>> next = nullptr;

            Node& operator=(const Node& rhs)
            {
                value = rhs.value;
                next = rhs.next;
            }

            std::string to_string() const
            {
                return std::to_string(value);
            }
        };


        class iterator : public std::iterator<std::forward_iterator_tag, std::shared_ptr<Node<T>>, long, std::shared_ptr<Node<T>>*, std::shared_ptr<Node<T>>&>
        {
        public:
            std::shared_ptr<Node<T>> start;
            explicit iterator(std::shared_ptr<Node<T>> _start) : start(_start) {}
            iterator operator++()
            {
                start = start->next;
                return *this;
            }

            bool operator!=(const iterator & other) const
            {
                return start != other.start;
            }

            const Node<T>& operator*() const
            {
                return *start;
            }
        };

        std::shared_ptr<Node<T>> head;
        std::shared_ptr<Node<T>> tail;
        std::size_t __length = 0;

    public:
        iterator begin() const {return iterator(head);}
        iterator end() const {return iterator(tail->next);}

        List() = default;
        List(const std::initializer_list<T>& values)
        {
            //TODO What happens with a empty init list
            auto begin = std::begin(values);
            auto end = std::end(values);

            head = std::make_shared<Node<T>>(*begin);
            ++begin;
            ++__length;
            std::shared_ptr<Node<T>> current = head;
            for(;begin != end; ++begin)
            {
                current->next = std::make_shared<Node<T>>(*begin);
                ++__length;
                current = current->next;
            }
            tail = current;
        }
        //TODO Flatten the recursion
        ~List() noexcept = default;

        List(const List& rhs) = default;

        List(List&& rhs) = delete;

        List& operator=(const List& rhs) noexcept
        {
            head = rhs.head;
            tail = rhs.tail;
            __length = rhs.length();
            return *this;
        }

        List& operator=(List&& rhs) noexcept
        {
            head = std::move(rhs.head);
            tail = std::move(rhs.tail);
            __length = std::move(rhs.length());
            return *this;
        }

        List deep_copy() &
        {
            List temp;

            size_t counter = 0;
            size_t length_ = length();

            auto val1 = this->operator[](counter);
            temp.head = std::make_shared<Node<T>>(val1);
            std::shared_ptr<Node<T>> current = temp.head;

            for(;counter < length_; ++counter)
            {
                auto val = this->operator[](counter);
                current->next = std::make_shared<Node<T>>(val);
                current = current->next;
            }
            temp.tail = current;
            temp.__length = length();
            return temp;
        }


        List deep_copy() &&
        {
            return this;
        }

        bool operator==(const List& rhs) const noexcept
        {
            if(length() != rhs.length()) return false;
            auto currentLhs = head;
            auto currentRhs = rhs.head;
            for(std::size_t i = 0; i < length(); ++i)
            {
                auto lhsVal = (*this)[i];
                auto rhsVal = rhs[i];
                if(lhsVal == rhsVal)
                {
                    ;
                }
                else
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const List& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        // operator++ concat
        const T& operator[](std::size_t counter) const
        {
            if(counter >= length())
            {
                throw std::out_of_range("Index is out of range");
            }
            std::size_t intCounter = 0;
            auto current = head;

            do
            {
                if(counter == intCounter)
                    break;
                ++intCounter;
            } while((current = current->next));

            return current->value;
        }

        //I think that the tail optimization prevents me from reusing data when inserting in the "middle" of the list
        //so the question whether the tail optimization is worth it or not
        List push_back(const T& value) const
        {
            //push_back on a list with max. length would overflow std::size_t __length
            if (length() == std::numeric_limits<std::size_t>::max())
            {
                throw std::overflow_error{"push_back on a list with max. length is not permitted"};
            }

            List temp;
            temp.__length = length();
            if(head)
            {
                temp.head = head;
                temp.tail = tail;
                temp.tail->next = std::make_shared<Node<T>>(value);
                ++temp.__length;
                //TODO Was soll das
                temp.tail = tail->next;
            }
            else
            {
                //This happens when we call push_back on an empty list
                //TODO HIER ISN BUG DENKE ICH
                temp.head = std::make_shared<Node<T>>(value);
                ++temp.__length;
                temp.tail = temp.head;
            }
            return temp;
        }

        List emplace_back(T&& value) const
        {
            ;
        }

        List push_front(const T& value) const
        {
            //push_front on a list with max. length would overflow std::size_t __length
            if (length() == std::numeric_limits<std::size_t>::max())
            {

                throw std::overflow_error{"push_front on a list with max. length is not permitted"};
            }

            List newList;
            newList.head = std::make_shared<Node<T>>(value);
            newList.__length = length();
            ++newList.__length;
            newList.head->next = head;
            newList.tail = tail;
            return newList;
        }

        List pop_front() const noexcept
        {
            List newList;
            newList.head = head->next;
            newList.tail = tail;
            return newList;
        }

        const Node<T>& getHead() const noexcept
        {
            //TODO I think this won't compile if T(head) is not copyable because std::make_optional copies its data.
            assert(length() >= 1);
            return head;
        }

        std::size_t length() const noexcept
        {
            return __length;
        }

    };

    //bitmapped vector tree/prefix tree
    template<typename T>
    class vector
    {
        
    };

}


template <typename F>
class lazy_val
{
private:
    F m_computation;
    mutable bool m_cache_initialized;
    mutable decltype(m_computation()) m_cache;
    mutable std::once_flag m_value_flag;
public:
    //Todo eliminate the need that Type(m_cache) must be default constructible
    lazy_val(F computation)
        : m_computation(computation)
        , m_cache_initialized(false)
    {
    }

    operator const decltype(m_computation())& () const
    {
        std::call_once(m_value_flag, [this] {
                m_cache = m_computation();
            });
        return m_cache;
    }
};

    //TODO Brauchen wir das da wir c++17 haben?
template <typename F>
inline lazy_val<F> make_lazy_val(F&& computation)
{
    return lazy_val<F>(std::forward<F>(computation));
}


}
#endif

