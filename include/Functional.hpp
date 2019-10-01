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
//TODO Remove volatile from types in all functions
//TODO TypeClasses for all container like inherit from functor or monad?
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
                temp = temp.push_back(it);
            }
        }
        return temp;
    }

    // TOTHINK replace std::bind with lambdas so the compiler cant optimize more easily
    template<typename C, typename T>
    std::function<T(const C& container)> sum = std::bind(fold<C, T, std::plus<T>>,std::placeholders::_1, std::plus<T>(),0);

    template<typename C, typename T>
    std::function<T(const C& container)> product = std::bind(fold<C, T, std::multiplies<T>>,std::placeholders::_1, std::multiplies<T>(),1);




    //push_back() && push_front() &&
    //TODO List::tail is wrong, it should return all the elements except the head not only the last
    //TODO make the forward iterator of list a biderictional iterator
    //TODO Supports a constructor which takes iterators
    //TODO Support swap, empty, reverse_iterators, size initializer, assign(Primer P. 338)
    template<typename T>
    class List
    {
    public:
        template<typename D>
        class Node
        {
        public:
            Node(T val) : value(val){}
            const T value;
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

            ~Node()
            {
                //TODO WHY DOES IT WORK
                auto next_node = std::move(next);
                while (next_node)
                {
                    //If I am not the sole owner of my tail don't do anything
                    if (!next_node.unique())
                        break;
                    std::shared_ptr<Node<T>> _tail;
                    std::swap(_tail, next_node->next);
                    next_node.reset();
                    next_node = std::move(_tail);
                }
            }
        };


        using value_type = Node<T>;

    private:
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

            const T& operator*() const
            {
                return start->value;
            }
        };

        std::shared_ptr<Node<T>> mHead;
        std::shared_ptr<Node<T>> mTail;
        std::size_t __length = 0;

    public:
        iterator cbegin() const {return iterator(mHead);}
        iterator cend() const {return iterator(mTail->next);}

        List() = default;
        List(const std::initializer_list<T>& values)
        {
            //TODO What happens with a empty init list
            auto begin = std::begin(values);
            auto end = std::end(values);
            //TODO Das sollte in die schleife
            mHead = std::make_shared<Node<T>>(*begin);
            ++begin;
            ++__length;
            std::shared_ptr<Node<T>> current = mHead;
            for(;begin != end; ++begin)
            {
                current->next = std::make_shared<Node<T>>(*begin);
                ++__length;
                current = current->next;
            }
            mTail = current;
        }
        //TODO Flatten the recursion
        ~List() = default;

        List(const List& rhs) = default;

        List(List&& rhs) = delete;

        List& operator=(const List& rhs) noexcept
        {
            mHead = rhs.mHead;
            mTail = rhs.mTail;
            __length = rhs.length();
            return *this;
        }

        List& operator=(List&& rhs) noexcept
        {
            mHead = std::move(rhs.mHead);
            mTail = std::move(rhs.mTail);
            __length = std::move(rhs.length());
            return *this;
        }

        List deep_copy() &
        {
            List temp;

            size_t counter = 0;
            size_t length_ = length();

            auto val1 = this->operator[](counter);
            temp.mHead = std::make_shared<Node<T>>(val1);
            std::shared_ptr<Node<T>> current = temp.mHead;

            for(;counter < length_; ++counter)
            {
                auto val = this->operator[](counter);
                current->next = std::make_shared<Node<T>>(val);
                current = current->next;
            }
            temp.mTail = current;
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
            auto currentLhs = mHead;
            auto currentRhs = rhs.mHead;
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
            auto current = mHead;

            do
            {
                if(counter == intCounter)
                    break;
                ++intCounter;
            } while((current = current->next));

            return current->value;
        }

        //TODO Find out how to optimize this for rvalues or don't because on O3 it's reansonly fast
        List push_back(const T& value) const
        {
            //push_back on a list with max. length would overflow std::size_t __length
            if (length() == std::numeric_limits<std::size_t>::max())
            {
                throw std::overflow_error{"push_back on a list with max. length is not permitted"};
            }

            List temp;
            temp.__length = length();
            if(mHead)
            {
                temp.mHead = mHead;
                temp.mTail = mTail;
                temp.mTail->next = std::make_shared<Node<T>>(value);
                ++temp.__length;
                //TODO Was soll das
                temp.mTail = mTail->next;
            }
            else
            {
                //This happens when we call push_back on an empty list
                //TODO HIER ISN BUG DENKE ICH
                temp.mHead = std::make_shared<Node<T>>(value);
                ++temp.__length;
                temp.mTail = temp.mHead;
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
            newList.mHead = std::make_shared<Node<T>>(value);
            newList.__length = length();
            ++newList.__length;
            newList.mHead->next = mHead;
            newList.mTail = mTail;
            return newList;
        }

        List pop_front() const noexcept
        {
            List newList;
            newList.mHead = mHead->next;
            newList.mTail = mTail;
            return newList;
        }

        const std::optional<T> head() const noexcept
        {
            //TODO I think this won't compile if T(mHead) is not copyable because std::make_optional copies its data.
            //SOLTUION std::optional uses some form of direct initialization but isn't this still expensive?
            return (mHead) ? std::make_optional<T>(mHead->value) : std::nullopt;
        }


        const std::optional<T> tail() const noexcept
        {
            //TODO I think this won't compile if T(mHead) is not copyable because std::make_optional copies its data.
            //SOLTUION std::optional uses some form of direct initialization but isn't this still expensive?
            return (mTail) ? std::make_optional<T>(mTail->value) : std::nullopt;
        }

        std::size_t length() const noexcept
        {
            return __length;
        }
    };

    template<typename T>
    class BST
    {
        struct Node
        {
            Node(std::shared_ptr<const Node> const & lft, T val, std::shared_ptr<const Node> const & rgt)
                : _lft(lft), _val(val), _rgt(rgt)
                {}
            std::shared_ptr<const Node> _lft;
            T      _val;
            std::shared_ptr<const Node> _rgt;
        };
        explicit BST(std::shared_ptr<const Node> const & node) : _root(node) {}
    public:
        BST() {}
        BST(BST const & lft, T val, BST const & rgt)
            : _root(std::make_shared<const Node>(lft._root, val, rgt._root))
        {
        }
        BST(std::initializer_list<T> init)
        {
            BST t;
            for (T v: init)
            {
                t = t.inserted(v);
            }
            _root = t._root;
        }
        bool isEmpty() const { return !_root; }
        T root() const
        {
            assert(!isEmpty());
            return _root->_val;
        }
        BST left() const
        {
            assert(!isEmpty());
            return BST(_root->_lft);
        }
        BST right() const
        {
            assert(!isEmpty());
            return BST(_root->_rgt);
        }
        bool member(T x) const
        {
            if (isEmpty())
                return false;
            T y = root();
            if (x < y)
                return left().member(x);
            else if (y < x)
                return right().member(x);
            else
                return true;
        }
        BST inserted(T x) const
        {
            //If I am empty construt a new Tree with me as the root
            if (isEmpty())
                return BST(BST(), x, BST());
            //Get the root for comparison
            T y = root();
            if (x < y)
                return BST(left().inserted(x), y, right());
            else if (y < x)
                return BST(left(), y, right().inserted(x));
            else
                return BST(left().inserted(x), y, right());
            }
    private:
        std::shared_ptr<const Node> _root;
    };

    template<class T, class F>
    void forEach(BST<T> t, F f)
    {
        if (!t.isEmpty())
        {
            forEach(t.left(), f);
            f(t.root());
            forEach(t.right(), f);
        }

    }



    template<typename T>
    class BST_IT
    {
        struct Node
        {
            Node(std::shared_ptr<const Node> const & lft, T val, std::shared_ptr<const Node> const & rgt)
                : lft(lft), val(val), rgt(rgt)
                {}
            std::shared_ptr<const Node> lft;
            T      val;
            std::shared_ptr<const Node> rgt;
        };

        explicit BST_IT(std::shared_ptr<const Node> const & node) : _root(node) {}
    public:
        BST_IT() {}
        BST_IT(BST_IT const & lft, T val, BST_IT const & rgt)
            : _root(std::make_shared<const Node>(lft._root, val, rgt._root))
        {
        }
        BST_IT(std::initializer_list<T> init)
        {
            BST_IT t;
            for (T v: init)
            {
                t = t.inserted(v);
            }
            _root = t._root;
        }
        bool isEmpty() const { return !_root; }
        const T& root() const
        {
            assert(!isEmpty());
            return _root->val;
        }
        BST_IT left() const
        {
            assert(!isEmpty());
            return BST_IT(_root->lft);
        }
        BST_IT right() const
        {
            assert(!isEmpty());
            return BST_IT(_root->rgt);
        }
        bool member(T x) const
        {
            if (isEmpty())
                return false;
            std::shared_ptr<const Node>& y = _root;

            while(y)
            {
                if(x < y->val)
                {
                    y = y->lft;
                }
                else if ( x > y->val)
                {
                    y = y->rgt;
                }
                else
                {
                    return true;
                }
            }
            return false;
        }
        BST_IT inserted(T x) const
        {
            //If I am empty construt a new Tree with me as the root
            if (isEmpty())
                return BST_IT(BST_IT(), x, BST_IT());
            //Get the root for comparison
            T y = root();
            if (x < y)
                return BST_IT(left().inserted(x), y, right());
            else if (y < x)
                return BST_IT(left(), y, right().inserted(x));
            else
                return BST_IT(left().inserted(x), y, right());
            }
    private:
        mutable std::shared_ptr<const Node> _root;
    };

    template<class T, class F>
    void forEach(BST_IT<T> t, F f)
    {
        if (!t.isEmpty())
        {
            forEach(t.left(), f);
            f(t.root());
            forEach(t.right(), f);
        }

    }
    //bitmapped vector tree/prefix tree
    template<typename T>
    class vector
    {
        
    };


    template <typename ArgType,
              typename F,
              typename Ret = typename std::result_of<F(ArgType)>::type
              >
    List<Ret> map(const List<ArgType> list,  F f)
    {
        //TODO Look at std::result_of page 214 in c++ func. programming
        static_assert(
            std::is_convertible<F,std::function<Ret(const ArgType)>>::value,
            "map requires a function of const  Ret (const ArgType)" //TODO Tut es nicht T1 (T2) waere korrekt
            );

        List<Ret> temp;
        //Container must support iteration
        for(typename List<ArgType>::value_type it: list)
        {
            //Container must be move assgnable
            //TODO: Think about how eliminate all the useless copies
            //SOLUTUION: Make a deep-copy and provide a map futions which is overloaded only for rvalues in the data structure
            //TODO Figure out how make this solution stl compatible
            temp = temp.push_back(f(it.value));
            // temp.push_back(f(it));
        }
        //Cotainer must be copyable
        return temp;
    }

    template <typename T,
              typename F>
    T fold(const func::pure::List<T>& list, F f, const T accu)
    {
        static_assert(
            std::is_convertible<F,std::function<T(const T, const T)>>::value,
            "fold requires a function of T (T, T)"
            );

        T accumulator = accu;
        for(const auto& it: list)
        {
            accumulator = f(it.value, accumulator);
        }
        return accumulator;
    }


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

