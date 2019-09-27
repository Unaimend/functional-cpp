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

    template <typename Container,
              typename ArgType,
              typename F>
    Container map(const Container& vector, F f)
    {
        static_assert(
            std::is_convertible<F,std::function<ArgType(const ArgType)>>::value,
            "map requires a function of T (T)" //TODO Tut es nicht T1 (T2) waere korrekt
            );

        Container temp;
        for(const ArgType& it: vector)
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

        /*
        class iterator : public std::iterator<std::forward_iterator_tag, Node<T>, long, Node<T>*, Node<T>&>
        {
        public:
            Node<T> start;
            explicit iterator(Node<T> _start) : start(_start) {}


        };
        */
        std::shared_ptr<Node<T>> head;
        std::shared_ptr<Node<T>> tail;
        std::size_t __length = 0;

    public:

        //iterator begin() {return head;}
        //iterator end() {return tail->next;}
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
        ~List() = default;

        List(const List& rhs) = default;

        List(List&& rhs) = delete;

        List& operator=(List& rhs)
        {
            head = rhs.head;
            tail = rhs.tail;
            __length == rhs.length();
            return *this;
        }

        List& operator=(List&& rhs) = delete;

        bool operator==(const List& rhs)
        {
            if(length() != rhs.length()) return false;
            auto currentLhs = head;
            auto currentRhs = rhs.head;
            while((currentLhs = currentLhs->next))
            {
                currentRhs = currentRhs->next;
                if(currentLhs->value != currentRhs->value)
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const List& rhs)
        {
            return !(*this == rhs);
        }

        // operator++
        const T& operator[](int counter) const
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

            std::cout << current->value << std::endl;
            return current->value;
        }

        //I think that the tail optimization prevents me from reusing data when inserting in the "middle" of the list
        //so the question whether the tail optimization is worth it or not
        List push_back(const T& value) const
        {
            List temp;
            temp.__length = length();
            if(head)
            {
                temp.head = head;
                temp.tail = tail;
                temp.tail->next = std::make_shared<Node<T>>(value);
                ++temp.__length;
                temp.tail = tail->next;
            } else
            {
                //This happens when we call push_back on an empty list
                temp.head = std::make_shared<Node<T>>(value);
                ++temp.__length;
                temp.tail = temp.head->next;
            }
            return temp;
        }

        List emplace_back(T&& value) const
        {
            ;
        }

        List push_front(const T& value) const
        {
            List newList;
            newList.head = std::make_shared<Node<T>>(value);
            newList.__length = length();
            ++newList.__length;
            newList.head->next = head;
            newList.tail = tail;
            return newList;
        }

        List remove_front() const
        {
            List newList;
            newList.head = head->next;
            newList.tail = tail;
            return newList;
        }

        const std::optional<Node<T>> getHead() const
        {
            //TODO I think this won't compile if T(head) is not copyable because std::make_optional copies its data.
            return head ? std::make_optional(head) : std::nullopt;
        }

        std::size_t length() const noexcept
        {
            return __length;
        }

        void print() const
        {
            auto current = head;
            std::cout << "Head: " << head->value << std::endl;
            while(current->next)
            {
                current = current->next;
                std::cout <<"Value: " << current->value << std::endl;
                if(current == tail)
                {
                    break;
                }
            }
            std::cout << "Tail: " << tail->value << std::endl;
        }

        
        
    };

    //bitmapped vector tree/prefix tree
    template<typename T>
    class vector
    {
        
    };

}


template <typename F>
class lazy_val {
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

