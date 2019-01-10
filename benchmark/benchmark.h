#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace bm
{
    class Benchmark
    {
    public:
        using TaskType = std::function<void()>;
        using Clock = std::chrono::high_resolution_clock;

        std::vector<std::string> task_names;
        std::vector<TaskType> tasks;
        std::vector<std::string> filter;

        struct RunResult
        {
            std::vector<std::string> task_names;
            std::vector<bool> ran;
            size_t repeat;
            std::vector<double> best;
            std::vector<double> avg;

            std::string to_string()
            {
                std::stringstream s;

                for (size_t i = 0; i < task_names.size(); i++)
                {
                    s << "{ ";
                    s << "\"task\": \"" << task_names[i] << "\", ";
                    s << "\"ran\": " << ran[i] << ", ";
                    s << "\"repeat\": " << repeat << ", ";
                    s << "\"best\": " << best[i] << ", ";
                    s << "\"avg\": " << avg[i];
                    s << " }" << std::endl;
                }

                return s.str();
            }
        };

        Benchmark(std::initializer_list<std::string> task_names, 
            std::initializer_list<TaskType> tasks,
            std::initializer_list<std::string> filter = {})
            : task_names(std::move(task_names)), tasks(std::move(tasks)), filter(std::move(filter))
        {
            assert(task_names.size() == tasks.size());
        }

        RunResult run(int repeat = 3)
        {
            size_t index = 0;
            RunResult result;
            result.task_names = task_names;
            result.repeat = repeat;

            for (const auto & task : tasks)
            {
                bool skip = false;
                for (const auto & name : filter)
                {
                    if (name == task_names[index])
                    {
                        result.ran.push_back(false);
                        result.avg.push_back(0);
                        result.best.push_back(0);
                        skip = true;
                        break;
                    }
                }
                if (skip) continue;

                result.ran.push_back(true);
                std::vector<double> repeat_times;
                for (int i = 0; i < repeat; i++)
                {
                    // std::cout << task_names[index] << " repeat: " << i << std::endl;
                    repeat_times.push_back(run_task(task));
                }

                result.best.push_back(*std::min_element(repeat_times.begin(), repeat_times.end()));
                result.avg.push_back(std::accumulate(repeat_times.begin(), repeat_times.end(), 0.0) / repeat);

                index += 1;
            }

            return result;
        }

        std::string get(int repeat = 3)
        {
            return run(repeat).to_string();
        }

        double run_task(const TaskType & task)
        {
            auto t1 = Clock::now();
            task();
            auto t2 = Clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() * 1.0 / 10e3;
        }

    };
}