#include <iostream>
#include "../include/Functional.hpp"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "../external/picobench/include/picobench/picobench.hpp"

#include <vector>
#include <list>
#include <cstdlib> // for rand

// Benchmarking function written by the user:
PICOBENCH_SUITE("Push_back");
static void rand_vector(picobench::state& s)
{
    std::vector<int> v;
    for (auto _ : s)
    {
        v.push_back(rand());
    }
}
PICOBENCH(rand_vector); // Register the above function with picobench

// Another benchmarking function:
static void rand_std_list(picobench::state& s)
{
    std::list<int> v;
    for (auto _ : s)
    {
        v.push_back(rand());
    }
}
PICOBENCH(rand_std_list);

static void rand_func_list(picobench::state& s)
{
    func::pure::List<int> v;
    for (auto _ : s)
    {
        v = v.push_back(rand());
    }
}

PICOBENCH(rand_func_list);

PICOBENCH_SUITE("Copying");
static void copy_std_vector(picobench::state& s)
{
    std::vector<int> v;
    for (auto _ : s)
    {
        v.push_back(rand());
    }
    s.start_timer();
    std::vector<int> v1;
    v1 = v;
    s.stop_timer();
}

PICOBENCH(copy_std_vector);
static void copy_std_list(picobench::state& s)
{
    std::list<int> v;
    for (auto _ : s)
    {
         v.push_back(rand());
    }
    s.start_timer();
    std::list<int> v1;
    v1 = v;
    s.stop_timer();
}

PICOBENCH(copy_std_list);

static void copy_func_list(picobench::state& s)
{
    func::pure::List<int> v;
    for (auto _ : s)
    {
        v = v.push_back(rand());
    }
    s.start_timer();
    func::pure::List<int> v1 ;
    v1 = v;
    s.stop_timer();
}

PICOBENCH(copy_func_list);

auto square = [](const int val)
{
    return val*val;
};


PICOBENCH_SUITE("Map square over the container");
static void map_std_vector(picobench::state& s)
{
    std::vector<int> v;
    for (auto _ : s)
    {
        v.push_back(rand());
    }
    s.start_timer();
    std::vector<int> v1 = func::pure::map(v, square);
    s.stop_timer();
}

PICOBENCH(map_std_vector);
static void map_std_list(picobench::state& s)
{
    std::list<int> v;
    for (auto _ : s)
    {
        v.push_back(rand());
    }
    s.start_timer();
    std::list<int> v1 = func::pure::map(v, square);
    s.stop_timer();
}

PICOBENCH(map_std_list);

static void map_func_list(picobench::state& s)
{
    func::pure::List<int> v;
    for (auto _ : s)
    {
        v = v.push_back(rand());
    }
    s.start_timer();
    func::pure::List<int> v2 = func::pure::map(v, square);
    s.stop_timer();
}

PICOBENCH(map_func_list);


/*int main()
{
    picobench::runner runner;
    // Disregard command-line for simplicity

    // Two sets of iterations
    runner.set_default_state_iterations({10000, 50000});

    // One sample per benchmark because the huge numbers are expected to compensate
    // for external factors
    runner.set_default_samples(1);

    // Run the benchmarks with some seed which guarantees the same order every time
    auto report = runner.run_benchmarks(123);

    // Output to some file
    report.to_csv(std::cout);

    return 0;
}
*/
