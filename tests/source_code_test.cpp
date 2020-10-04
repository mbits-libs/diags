// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <diags/sources.hpp>
#include <numeric>
#include <support/raven_text.hpp>

extern std::filesystem::path TESTING_data_path;

static auto const the_raven_length =
    std::transform_reduce(std::begin(the_raven),
                          std::end(the_raven),
                          size_t{},
                          std::plus<>{},
                          [](auto const& view) { return view.length(); }) +
    std::size(the_raven) - 1;

namespace diags::testing {
	TEST(source_code, data) {
		std::string expected;
		expected.reserve(the_raven_length);
		for (auto const& line : the_raven) {
			expected.append(line);
			if (expected.size() < the_raven_length) expected.push_back('\n');
		}

		sources host{};

		auto code = host.open((TESTING_data_path / "the_raven.txt").string());

		ASSERT_TRUE(code.valid());

		auto contents = code.data();
		auto actual = std::string{
		    reinterpret_cast<char const*>(contents.data()), contents.size()};

		EXPECT_EQ(expected, actual);
	}

	TEST(source_code, read_one_line) {
		sources host{};
		auto code = host.open((TESTING_data_path / "the_raven.txt").string());

		ASSERT_TRUE(code.valid());

		std::string_view line = the_raven[0];
		while (!line.empty()) {
			if (code.peek() == std::byte{'\n'}) break;

			char expected = line[0];
			line = line.substr(1);

			char actual;
			auto read = code.read(&actual, sizeof actual);

			EXPECT_EQ(1, read);
			EXPECT_EQ(expected, actual);
		}

		EXPECT_TRUE(line.empty());
		EXPECT_EQ('\n', static_cast<char>(code.peek()));
		EXPECT_FALSE(code.eof());
	}

	TEST(source_code, seek_10MiB) {
		auto const expected = the_raven_length;

		sources host{};
		auto code = host.open((TESTING_data_path / "the_raven.txt").string());

		ASSERT_TRUE(code.valid());

		auto pos_seek = code.seek(10 * 1024 * 1024);
		auto pos_tell = code.tell();

		uint32_t nothing;
		auto read = code.read(&nothing, sizeof nothing);

		EXPECT_EQ(expected, pos_seek);
		EXPECT_EQ(expected, pos_tell);
		EXPECT_EQ(0, read);
	}
}  // namespace diags::testing
