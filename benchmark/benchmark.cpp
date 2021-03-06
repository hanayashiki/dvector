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
            }).get(3);
    }

    {
        auto dvector_insert_erase_repeater = [](int repeat)
        {
            return [=]() 
            {

                dv::dvector<int> d;
                std::default_random_engine e;
                e.seed(0);

                for (int i = 0; i < repeat; i++)
                {
                    d.insert(d.size(), 1);
                }

                for (int i = 0; i < repeat; i++)
                {
                    d.erase(e() % d.size());
                }
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

    {
        auto dvector_random_insert_read = [](int insert_repeat, int read_repeat) {
            return [=]() {
                dv::dvector<int> d({ 0 });
                std::default_random_engine e;
                e.seed(0);

                for (int i = 0; i < insert_repeat; i++)
                {
                    d.insert(e() % d.size(), 1);
                }

                for (int i = 0; i < read_repeat; i++)
                {
                    auto elem = d[e() % d.size()];
                }
            };
        };

        auto vector_random_insert_read = [](int insert_repeat, int read_repeat) {
            return [=]() {
                std::vector<int> d{ 0 };
                std::default_random_engine e;
                e.seed(0);

                for (int i = 0; i < insert_repeat; i++)
                {
                    d.insert(d.begin() + e() % d.size(), 1);
                }

                for (int i = 0; i < read_repeat; i++)
                {
                    auto elem = d[e() % d.size()];
                }
            };
        };

        std::cout << Benchmark(
            { "dvector_random_insert_read_1000", "vector_random_insert_read_1000" },
            {
                dvector_random_insert_read(1000, 1000), vector_random_insert_read(1000, 1000)
            },
            {
                //  "vector_insert_erase_10000"
            }).get(3);

        std::cout << Benchmark(
            { "dvector_random_insert_read_10000", "vector_random_insert_read_10000" },
            {
                dvector_random_insert_read(10000, 10000), vector_random_insert_read(10000, 10000)
            },
            {
                //  "vector_insert_erase_10000"
            }).get(3);

        std::cout << Benchmark(
            { "dvector_random_insert_read_100000", "vector_random_insert_read_100000" },
            {
                dvector_random_insert_read(100000, 100000), vector_random_insert_read(100000, 100000)
            },
            {
                //  "vector_insert_erase_10000"
            }).get(3);

        std::cout << Benchmark(
            { "dvector_random_insert_read_200000", "vector_random_insert_read_200000" },
            {
                dvector_random_insert_read(200000, 200000), vector_random_insert_read(200000, 200000)
            },
            {
                //  "vector_insert_erase_10000"
            }).get(3);
    }


    std::cout << "All benchmarks are run" << std::endl;
    std::cin.get();
}

