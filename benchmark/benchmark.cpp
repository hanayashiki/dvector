// benchmark.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "../dvector/dvector.h"
#include "benchmark.h"

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
            },
            {
                "vector_push_back_10000"
            }).get(1000);
    }


    std::cout << "All benchmarks are run" << std::endl;
    std::cin.get();
}

