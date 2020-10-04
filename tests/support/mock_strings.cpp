// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <support/mixed_types.hpp>
#include <support/plural_types.hpp>
#include <support/singular_types.hpp>

using namespace std::literals;

#define ALT_SINGLE(NAME) \
	case lng::NAME:      \
		return #NAME##sv
#define ALT_SINGLE_(NAME, VIEW) \
	case lng::NAME:             \
		return VIEW

#define ALT_COUNTED(NAME) \
	case counted::NAME:   \
		return select(count, #NAME "[ONE]"sv, #NAME "[{0}]"sv)

#define ALT_COUNTED_(NAME, ONE, MANY) \
	case counted::NAME:               \
		return select(count, ONE, MANY)

std::string_view select(intmax_t count,
                        std::string_view alt1,
                        std::string_view alt2) noexcept {
	return count == 1 ? alt1 : alt2;
}

namespace diags::testing::singular {
	std::string_view strings_mock::get(lng id) const noexcept {
		switch (id) {
			case lng::MESSAGE_A:
				return "message A (singular)";
			case lng::MESSAGE_B:
				return "message B (singular)";
			case lng::MESSAGE_C:
				return "message C with {0} and {2} (singular)";
			case lng::ARG_1:
				return "argument-1";
			case lng::ARG_2:
				return "argument-2";
		};
		return {};
	}

	std::string_view alt_strings_mock::get(lng id) const noexcept {
		switch (id) {
			ALT_SINGLE(MESSAGE_A);
			ALT_SINGLE(MESSAGE_B);
			ALT_SINGLE_(MESSAGE_C, "MESSAGE_C({0}, {2})");
			ALT_SINGLE(ARG_1);
			ALT_SINGLE(ARG_2);
		}
		return "";
	}
}  // namespace diags::testing::singular

namespace diags::testing::plural {
	std::string_view strings_mock::get(counted id,
	                                   intmax_t count) const noexcept {
		switch (id) {
			case counted::MESSAGE_A:
				return select(count, "message A (plural)",
				              "message A ({} plurals)");
			case counted::MESSAGE_B:
				return select(count, "message B (plural)",
				              "message B ({} plurals)");
			case counted::MESSAGE_C:
				return select(count, "message C with {0} and {2} (plural)",
				              "message C with {0} and {2} ({3} plurals)");
			case counted::ARG_1:
				return select(count, "single argument-1", "{0} arguments-1");
			case counted::ARG_2:
				return select(count, "single argument-2", "{0} arguments-2");
			case counted::MANY_3:
				return select(count, "one foo", "{0} foos");
			case counted::MANY_4:
				return select(count, "{0} bar", "many bars");
			case counted::MANY_5:
				return select(count, "one baz", "{} bazes");
		}
		return "";
	}

	std::string_view alt_strings_mock::get(counted id,
	                                       intmax_t count) const noexcept {
		switch (id) {
			ALT_COUNTED(MESSAGE_A);
			ALT_COUNTED(MESSAGE_B);
			ALT_COUNTED_(MESSAGE_C, "MESSAGE_C[ONE]({0}, {2})",
			             "MESSAGE_C[{3}]({0}, {2})");
			ALT_COUNTED(ARG_1);
			ALT_COUNTED(ARG_2);
			ALT_COUNTED(MANY_3);
			ALT_COUNTED(MANY_4);
			ALT_COUNTED(MANY_5);
		}
		return "";
	}
}  // namespace diags::testing::plural

namespace diags::testing::mixed {
	std::string_view strings_mock::get(lng id) const noexcept {
		switch (id) {
			case lng::MESSAGE_A:
				return "message A (singular)";
			case lng::MESSAGE_B:
				return "message B (singular)";
			case lng::MESSAGE_C:
				return "message C with {0} and {2} (singular)";
			case lng::ARG_1:
				return "argument-1";
			case lng::ARG_2:
				return "argument-2";
		};
		return {};
	}

	std::string_view strings_mock::get(counted id,
	                                   intmax_t count) const noexcept {
		switch (id) {
			case counted::MANY_3:
				return select(count, "one foo", "{0} foos");
			case counted::MANY_4:
				return select(count, "{0} bar", "many bars");
			case counted::MANY_5:
				return select(count, "one baz", "{} bazes");
		}
		return "";
	}

	std::string_view alt_strings_mock::get(lng id) const noexcept {
		switch (id) {
			ALT_SINGLE(MESSAGE_A);
			ALT_SINGLE(MESSAGE_B);
			ALT_SINGLE_(MESSAGE_C, "MESSAGE_C({0}, {2})");
			ALT_SINGLE(ARG_1);
			ALT_SINGLE(ARG_2);
		}
		return "";
	}

	std::string_view alt_strings_mock::get(counted id,
	                                       intmax_t count) const noexcept {
		switch (id) {
			ALT_COUNTED(MANY_3);
			ALT_COUNTED(MANY_4);
			ALT_COUNTED(MANY_5);
		}
		return "";
	}
}  // namespace diags::testing::mixed
