// benchmark.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "../dvector/dvector.h"
#include "benchmark.h"
#include <random>

#include <iostream>

int main()
{
    using namespace bm;
    
    {
        auto dvector_push_back_10000 = []() {
            return [](int repeat)
            {
                dv::dvector<int> d(std::vector<int>{});
                for (int i = 0; i < repeat; i++)
                {
                    d.insert(d.size(), 1);
                }
            }(10000);
        };

        auto vector_push_back_10000 = []() {
            return [](int repeat)
            {
                std::vector<int> d;
                for (int i = 0; i < repeat; i++)
                {
                    d.push_back(1);
                }
            }(10000);
        };
        
        std::cout << Benchmark(
            { "dvector_push_back_10000", "vector_push_back_10000" },
            {
                dvector_push_back_10000, vector_push_back_10000
            }).get(1000);
    }

    {
        auto dvector_insert_erase_repeater = [](int repeat)
        {
            return [=]() 
            {
                [](int r)
                {
                    dv::dvector<int> d;
                    std::default_random_engine e;
                    e.seed(0);

                    for (int i = 0; i < r; i++)
                    {
                        d.insert(d.size(), 1);
                    }

                    for (int i = 0; i < r; i++)
                    {
                        d.erase(e() % d.size());
                    }
                }(repeat);
            };
        };

        auto vector_insert_erase_repeater = [](int repeat)
        {
            return [=]() {
                [](int repeat)
                {
                    std::vector<int> d;
                    std::default_random_engine e;
                    e.seed(0);

                    for (int i = 0; i < repeat; i++)
                    {
                        d.push_back(1);
                    }

                    for (int i = 0; i < repeat; i++)
                    {
                        d.erase(d.begin() + e() % d.size());
                    }
                }(100000);
            };
        };

        std::cout << Benchmark(
            { "dvector_insert_erase_1000", "vector_insert_erase_1000" },
            {
                dvector_insert_erase_repeater(1000), vector_insert_erase_repeater(1000)
            },
            {
                //  "vector_insert_erase_10000"
            }).get(3);

        std::cout << Benchmark(
            { "dvector_insert_erase_10000", "vector_insert_erase_10000" },
            {
                dvector_insert_erase_repeater(10000), vector_insert_erase_repeater(10000)
            },
            {
                //  "vector_insert_erase_10000"
            }).get(3);

        std::cout << Benchmark(
            { "dvector_insert_erase_100000", "vector_insert_erase_100000" },
            {
                dvector_insert_erase_repeater(100000), vector_insert_erase_repeater(100000)
            },
            {
              //  "vector_insert_erase_10000"
            }).get(3);
    }


    std::cout << "All benchmarks are run" << std::endl;
    std::cin.get();
}

