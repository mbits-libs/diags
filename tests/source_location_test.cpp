// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <diags/sources.hpp>
#include <numeric>
#include <string_view>

namespace diags::testing {
	using namespace ::std::literals;
	using ::testing::Test;
	using ::testing::TestWithParam;
	using ::testing::ValuesIn;

	class source_location : public Test {
	protected:
		struct item {
			std::string_view path{};
			location loc{};
		};

		std::vector<item> data = {
		    // GCC
		    {"/usr/include/c++/8"sv},
		    {"/usr/include/x86_64-linux-gnu/c++/8"sv},
		    {"/usr/include/c++/8/backward"sv},
		    {"/usr/lib/gcc/x86_64-linux-gnu/8/include"sv},
		    {"/usr/local/include"sv},
		    {"/usr/lib/gcc/x86_64-linux-gnu/8/include-fixed"sv},
		    {"/usr/include/x86_64-linux-gnu"sv},
		    {"/usr/include"sv},
		    // Clang
		    {"/usr/include/c++/v1"sv},
		    {"/usr/include/clang/7.0.0/include"sv},
		    {"/usr/local/include"sv},
		    {"/usr/lib/gcc/x86_64-linux-gnu/8/include"sv},
		    {"/usr/include/x86_64-linux-gnu"sv},
		    {"/usr/include"sv},
		};
		sources host;

	public:
		void SetUp() override {
			for (auto& name : data) {
				name.loc = host.source(name.path).position();
			}
		}
	};

	TEST_F(source_location, unique) {
		for (auto begin_iter = begin(data), end_iter = end(data),
		          lhs_iter = begin_iter;
		     lhs_iter != end_iter; ++lhs_iter) {
			auto const& lhs = *lhs_iter;
			for (auto rhs_iter = begin_iter; rhs_iter != lhs_iter; ++rhs_iter) {
				auto const& rhs = *rhs_iter;

				if (lhs.path == rhs.path) {
					ASSERT_EQ(lhs.loc.token, rhs.loc.token);
				} else {
					ASSERT_NE(lhs.loc.token, rhs.loc.token);
				}
			}
		}
	}

	TEST_F(source_location, reverse) {
		for (auto const& name : data) {
			ASSERT_EQ(name.path, host.filename(name.loc));
		}
	}

	TEST_F(source_location, const_lookup) {
		sources local;
		sources const& ref = local;

		auto src = ref.source("");
		EXPECT_EQ(0u, src.position().token);
		EXPECT_EQ(""sv, ref.filename({2}));
	}
}  // namespace diags::testing