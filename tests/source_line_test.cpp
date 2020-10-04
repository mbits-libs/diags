// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <diags/sources.hpp>
#include <support/raven_text.hpp>

extern std::filesystem::path TESTING_data_path;

namespace diags::testing {
	using ::testing::TestWithParam;
	using ::testing::ValuesIn;

	TEST(source_line, lines) {
		sources host;
		auto src = host.open((TESTING_data_path / "the_raven.txt").string());

		EXPECT_TRUE(src.valid());
		EXPECT_EQ(""sv, src.line(0)) << "Line: 0";

		for (unsigned ln = 2 * static_cast<unsigned>(std::size(the_raven));
		     ln > 0; --ln) {
			if (ln <= std::size(the_raven)) {
				auto const line = ln - 1;
				EXPECT_EQ(src.line(ln), the_raven[line]) << "Line: " << ln;
			} else {
				EXPECT_EQ(src.line(ln), ""sv) << "Line: " << ln;
			}
		}
	}

	TEST(source_line, lines_not_found) {
		sources host;
		auto src = host.open((TESTING_data_path / "no-such.txt").string());

		EXPECT_FALSE(src.valid());
		EXPECT_EQ(src.line(0), ""sv) << "Line: 0";

		for (unsigned ln = 12; ln > 0; --ln) {
			EXPECT_EQ(src.line(ln), ""sv) << "Line: " << ln;
		}
		EXPECT_TRUE(src.valid());
	}
}  // namespace diags::testing
