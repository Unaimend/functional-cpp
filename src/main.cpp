#include <vector>
#include <iostream>
#include <list>
#include "../include/Functional.hpp"

struct A
{
    int a = 5;
    A(int _a):a(_a){};
    A& operator++()
        {
            ++a;
            return *this;
        }
};

struct B
{
    B(std::vector<int*> _data) : data(_data)
    {
        ;
    }

    void cant_change() const
    {
        *(data[1]) = 2;
        for(const auto& it : data)
        {
            std::cout << *it << std::endl;
        }
    }
private:
    std::vector<int*> data;
};

void iinc(int& a)
{
    a++;
}




const int inc(const int a)
{
    return a+1;
}

int add(const int a, const int b)
{
    return a + b;
}

void first_second(int first, int second)
{
    std::cout << "First: " << first << " Second: " << second << std::endl;
}

void first_second_ref(int first, int& second)
{
    std::cout << "First: " << first << " Second: " << ++second << std::endl;
}




int main(int argc, char const *argv[])
{
/*
std::vector<int> a{1,2,3,4,5};
func::inplace::map(a, iinc);
for(const auto& it : a)
    std::cout << "inplace inc: "  << it << std::endl;

std::vector<int> b = func::pure::map<std::vector<int>, int>(a, inc);
for(const auto& it : b)
    std::cout << "pure inc: " << it << std::endl;

std::vector<int> to_add_vec{1,2,3,4,5};
int result_vec = func::pure::fold<std::vector<int>, int>(to_add_vec, add, 0);
std::cout <<"result_vec: " << result_vec << std::endl;

std::list<int> to_add_list{1,2,3,4,5};


std::list<int> c = func::pure::map<int>(to_add_list, inc);

auto result_list = func::pure::fold(to_add_list, add, 0);
std::cout <<"result_list: " << result_list << std::endl;


auto result2 = func::pure::sum<std::vector<int>, int>(to_add_vec);
auto result3 = func::pure::product<std::vector<int>, int>(to_add_vec);
std::cout <<"result_sum: " << result2 << std::endl;


auto rev = func::pure::flip<void, int, int>(first_second);
auto rev_ref = func::pure::flip<void, int, int&>(first_second_ref);
rev(1,2);
int temp = 5;
int temp2 = 1;
rev_ref(temp2, temp);
std::cout << temp2 << " " <<  temp << std::endl;


auto is_positive = [](int test){return test > 0 ? true : false;};
auto is_positive_int = [](int test){return test > 0 ? 1: 0;};
std::vector<int> to_test = {1,2,3,-4,-1, 2, 4};
std::vector<int> after = func::pure::filter<int>(to_test, is_positive);


for(const auto& it : after)
    std::cout << "Filter"<< it << std::endl;
*/
    using namespace func::pure;
    Maybe test{5};
    Maybe test1 = 4;

    auto test3 = test.fmap(inc);

puts("-----------------------------------------------------------------");
//auto view = nano::views::filter(l5, even);
/*std::vector<int> ints{0,1,2,3,4,5};
auto square = [](int i) { return i * i; };

for (int i : ints | nano::views::filter(even) | nano::views::transform(square))
{
    std::cout << i << ' ';
}


std::cout << "ar" << ar << std::endl;*/
/*
for(const auto& it: l6)
{
    std::cout << it.value << std::endl;
}

for(const auto& it: l7)
{
    std::cout << it << std::endl;
}*/
}




