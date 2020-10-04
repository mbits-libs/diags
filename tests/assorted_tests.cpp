// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <diags/version.hpp>
#include <filesystem>

namespace diags::testing {
	using namespace ::std::literals;
	using ::testing::TestWithParam;
	using ::testing::ValuesIn;

	TEST(Assorted, Version) {
		EXPECT_EQ(version, get_version());
		EXPECT_TRUE(version.compatible_with(get_version()));
	}
}  // namespace diags::testing
