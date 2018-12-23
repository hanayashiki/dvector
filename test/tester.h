#pragma once

#include <sstream>
#include <string>
#include <algorithm>

#include <iostream>

#include <functional>
#include <cctype>

#define FLAG static inline const flag
#define CUT "=========================" 

namespace test_utils
{
	class tester
	{
	private:
		const std::string test_name;
		std::stringstream test_stream;
		std::stringstream expected_stream;

		auto filter(const std::string & s, std::function<bool(int)> pred)
		{
			std::string out;
			std::copy_if(s.begin(), s.end(), out.begin(), pred);
			return out;
		}

	public:
		typedef uint64_t flag;
		FLAG NO_FLAG = 0x0;
		FLAG NOT_IGNORE_SPACE = 0x1;
		tester(const std::string & test_name) : test_name(test_name) {
			std::cerr << "Test: " << test_name << std::endl;
		}
		~tester() {
			compare();
		}

		auto & test() { return test_stream; }
		auto & expected() { return expected_stream; }

		void clear() { 
			test_stream = std::stringstream();
			expected_stream = std::stringstream();
		}

		void compare(flag flags = NO_FLAG)
		{
			std::string
				test = test_stream.str(),
				expected = expected_stream.str();

			auto raw_test = test;
			auto raw_expected = expected;

			if (flags & NOT_IGNORE_SPACE)
			{
				std::tie(test, expected) = std::make_tuple(
					filter(test, isspace), 
					filter(expected, isspace));
			}

			if (test != expected)
			{
				std::cerr << "Test failure! " << std::endl;
				std::cerr << "Actual: " << std::endl;
				std::cerr << CUT << std::endl;
				std::cerr << raw_test << std::endl;
				std::cerr << CUT << std::endl;
				std::cerr << "Expected: " << std::endl;
				std::cerr << CUT << std::endl;
				std::cerr << raw_expected << std::endl;
				std::cerr << CUT << std::endl;

				throw std::exception("Test failure.");
			}
			else
			{
				std::cerr << CUT << std::endl;
				std::cerr << "passed" << std::endl;
				std::cerr << CUT << std::endl;
			}
		}

		void assert_exception(std::function<void()> routine)
		{
			try
			{
				routine();
			}
			catch (std::exception & e)
			{
				std::cerr << CUT << std::endl;
				std::cerr << "expected exception thrown: " << e.what() << std::endl;
				std::cerr << CUT << std::endl;
				return;
			}
			std::cerr << CUT << std::endl;
			std::cerr << "exception expected to throw here !" << std::endl;
			std::cerr << CUT << std::endl;
			throw std::exception("Test failure.");
		}
	};
}