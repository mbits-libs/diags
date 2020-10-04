// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <sstream>

#include <support/mixed_types.hpp>
#include <support/plural_types.hpp>
#include <support/printer.hpp>
#include <support/singular_types.hpp>

using namespace std::literals;

namespace diags::testing {
	using ::testing::ValuesIn;

	struct arg_builder {
		diagnostic diag;
		char const* formatted{""};
	};

	struct diagnostic_builder : ::testing::TestWithParam<arg_builder> {};

	TEST_P(diagnostic_builder, compare) {
		auto const& [diag, expected] = GetParam();

		auto actual = [&] {
			std::ostringstream oss;
			info(oss, diag);
			return oss.str();
		}();

		EXPECT_EQ(expected, actual);
	}

	constexpr location loc(unsigned line, unsigned column = 0) {
		return location{}.moved_to(line, column);
	}

	namespace singular {
		// parenthesis are here to switch the minus from location -
		// location_severity to location - location, and then use
		// location_range::operator[]
		static arg_builder diagnostics[] = {
		    {
		        loc(1, 1)[severity::error] << "lng::MESSAGE_A",
		        "(1:1 - 0:0)[severity::error] << \"lng::MESSAGE_A\"",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << "lng::MESSAGE_A",
		        "(1:1 - 1:10)[severity::error] << \"lng::MESSAGE_A\"",
		    },
		    {
		        loc(1, 1)[severity::error] << "lng::MESSAGE_A"s,
		        "(1:1 - 0:0)[severity::error] << \"lng::MESSAGE_A\"",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << "lng::MESSAGE_A"s,
		        "(1:1 - 1:10)[severity::error] << \"lng::MESSAGE_A\"",
		    },
		    {
		        loc(1, 1)[severity::error] << lng::MESSAGE_A,
		        "(1:1 - 0:0)[severity::error] << [501]",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << lng::MESSAGE_A,
		        "(1:1 - 1:10)[severity::error] << [501]",
		    },
		    {
		        loc(1, 1)[severity::error] << format("-{}-", lng::ARG_1),
		        "(1:1 - 0:0)[severity::error] << \"-{}-\"([504])",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << format("-{}-", lng::ARG_1),
		        "(1:1 - 1:10)[severity::error] << \"-{}-\"([504])",
		    },
		};

		INSTANTIATE_TEST_SUITE_P(singular,
		                         diagnostic_builder,
		                         ValuesIn(diagnostics));
	}  // namespace singular

	namespace plural {
		static arg_builder diagnostics[] = {
		    {
		        loc(1, 1)[severity::error] << counted::MESSAGE_A / 1,
		        "(1:1 - 0:0)[severity::error] << [501/1]",
		    },
		    {
		        loc(1, 1) - loc(1, 1).moved_by(0, 10)[severity::error]
		            << counted::MESSAGE_A / 5,
		        "(1:1 - 1:11)[severity::error] << [501/5]",
		    },
		    {
		        loc(1, 1)[severity::error]
		            << format("-{}-", counted::ARG_1 / 3),
		        "(1:1 - 0:0)[severity::error] << \"-{}-\"([504/3])",
		    },
		    {
		        loc(1, 1) - loc(1, 1).moved_by(0, 10)[severity::error]
		            << format("-{}-", counted::ARG_1 / 3),
		        "(1:1 - 1:11)[severity::error] << \"-{}-\"([504/3])",
		    },
		};

		INSTANTIATE_TEST_SUITE_P(plural,
		                         diagnostic_builder,
		                         ValuesIn(diagnostics));
	}  // namespace plural

	namespace mixed {
		static arg_builder diagnostics[] = {
		    {
		        loc(1, 1)[severity::error] << lng::MESSAGE_A,
		        "(1:1 - 0:0)[severity::error] << [501]",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << lng::MESSAGE_A,
		        "(1:1 - 1:10)[severity::error] << [501]",
		    },
		    {
		        loc(1, 1)[severity::error] << format("-{}-", lng::ARG_1),
		        "(1:1 - 0:0)[severity::error] << \"-{}-\"([504])",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << format("-{}-", lng::ARG_1),
		        "(1:1 - 1:10)[severity::error] << \"-{}-\"([504])",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << format("-{}-", counted::MANY_3 / 1),
		        "(1:1 - 1:10)[severity::error] << \"-{}-\"([506/1])",
		    },
		    {
		        (loc(1, 1) - loc(1, 1).moved_to_col(10))[severity::error]
		            << counted::MANY_3 / 1,
		        "(1:1 - 1:10)[severity::error] << [506/1]",
		    },
		};

		INSTANTIATE_TEST_SUITE_P(mixed,
		                         diagnostic_builder,
		                         ValuesIn(diagnostics));
	}  // namespace mixed
}  // namespace diags::testing
