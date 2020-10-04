// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <diag_write_common.hpp>
#include <support/plural_types.hpp>

namespace diags::testing::plural {
	class diags_write_plural
	    : public DiagsWriteTest<diagnostic_str, strings, translator_type> {};

	TEST_P(diags_write_plural, print) { print_test(); }
	TEST_P(diags_write_plural, dbg) { dbg_test(); }
	TEST_P(diags_write_plural, print_with_source) { print_with_source_test(); }
	TEST_P(diags_write_plural, has_errors) { has_errors_test(); }

	struct diags_cmp_plural : DiagsCmpTest<argument> {};

	TEST_P(diags_cmp_plural, comp) { comp_test(); }

	TEST(diags_write, plural_purecall) {
		strings_mock strings{};
		auto tr = strings.make();
		noenum::string::singular failing{10000};
		EXPECT_THROW(failing.translated(tr), std::runtime_error);
	}

	using diagnostic_write = testing::diagnostic_write<diagnostic_str>;

	static const diagnostic_write severities[] = {
	    {{"", 0, 0, severity::verbose, "", link_type::gcc, false}, {"\n"}},
	    {{"", 0, 0, severity::note, "", link_type::gcc, false}, {"note: \n"}},
	    {{"", 0, 0, severity::warning, "", link_type::gcc, false},
	     {"warning: \n"}},
	    {{"", 0, 0, severity::error, "", link_type::gcc, false}, {"error: \n"}},
	    {{"", 0, 0, severity::fatal, "", link_type::gcc, false}, {"fatal: \n"}},
	    {{""}, {"\n"}},
	    {{"", 0, 0, severity::note}, {"nnn: \n"}},
	    {{"", 0, 0, severity::warning}, {"www: \n"}},
	    {{"", 0, 0, severity::error}, {"eee: \n"}},
	    {{"", 0, 0, severity::fatal}, {"fff: \n"}},
	};

	static const diagnostic_write path[] = {
	    {{"path"}, {"\n"}},
	    {{"path", 0, 0, severity::note}, {"path: nnn: \n"}},
	    {{"path", 0, 0, severity::warning}, {"path: www: \n"}},
	    {{"path", 0, 0, severity::error}, {"path: eee: \n"}},
	    {{"path", 0, 0, severity::fatal}, {"path: fff: \n"}},
	    {{"path", 4}, {"\n"}},
	    {{"path", 4, 0, severity::note}, {"path:4: nnn: \n"}},
	    {{"path", 4, 0, severity::warning}, {"path:4: www: \n"}},
	    {{"path", 4, 0, severity::error}, {"path:4: eee: \n"}},
	    {{"path", 4, 0, severity::fatal}, {"path:4: fff: \n"}},
	    {{"path", 0, 2}, {"\n"}},
	    {{"path", 0, 2, severity::note}, {"path: nnn: \n"}},
	    {{"path", 0, 2, severity::warning}, {"path: www: \n"}},
	    {{"path", 0, 2, severity::error}, {"path: eee: \n"}},
	    {{"path", 0, 2, severity::fatal}, {"path: fff: \n"}},
	    {{"path", 4, 2}, {"\n"}},
	    {{"path", 4, 2, severity::note}, {"path:4:2: nnn: \n"}},
	    {{"path", 4, 2, severity::warning}, {"path:4:2: www: \n"}},
	    {{"path", 4, 2, severity::error}, {"path:4:2: eee: \n"}},
	    {{"path", 4, 2, severity::fatal}, {"path:4:2: fff: \n"}},
	    {{"path", 0, 0, severity::verbose, {}, link_type::vc}, {"\n"}},
	    {{"path", 0, 0, severity::note, {}, link_type::vc}, {"path: nnn: \n"}},
	    {{"path", 0, 0, severity::warning, {}, link_type::vc},
	     {"path: www: \n"}},
	    {{"path", 0, 0, severity::error, {}, link_type::vc}, {"path: eee: \n"}},
	    {{"path", 0, 0, severity::fatal, {}, link_type::vc}, {"path: fff: \n"}},
	    {{"path", 4, 0, severity::verbose, {}, link_type::vc}, {"\n"}},
	    {{"path", 4, 0, severity::note, {}, link_type::vc},
	     {"path(4): nnn: \n"}},
	    {{"path", 4, 0, severity::warning, {}, link_type::vc},
	     {"path(4): www: \n"}},
	    {{"path", 4, 0, severity::error, {}, link_type::vc},
	     {"path(4): eee: \n"}},
	    {{"path", 4, 0, severity::fatal, {}, link_type::vc},
	     {"path(4): fff: \n"}},
	    {{"path", 0, 2, severity::verbose, {}, link_type::vc}, {"\n"}},
	    {{"path", 0, 2, severity::note, {}, link_type::vc}, {"path: nnn: \n"}},
	    {{"path", 0, 2, severity::warning, {}, link_type::vc},
	     {"path: www: \n"}},
	    {{"path", 0, 2, severity::error, {}, link_type::vc}, {"path: eee: \n"}},
	    {{"path", 0, 2, severity::fatal, {}, link_type::vc}, {"path: fff: \n"}},
	    {{"path", 4, 2, severity::verbose, {}, link_type::vc}, {"\n"}},
	    {{"path", 4, 2, severity::note, {}, link_type::vc},
	     {"path(4,2): nnn: \n"}},
	    {{"path", 4, 2, severity::warning, {}, link_type::vc},
	     {"path(4,2): www: \n"}},
	    {{"path", 4, 2, severity::error, {}, link_type::vc},
	     {"path(4,2): eee: \n"}},
	    {{"path", 4, 2, severity::fatal, {}, link_type::vc},
	     {"path(4,2): fff: \n"}},
	};

	static const diagnostic_write message[] = {
	    {{"", 0, 0, severity::verbose, "a message"},
	     {"a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::note, "a message"},
	     {"nnn: a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::warning, "a message"},
	     {"www: a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::error, "a message"},
	     {"eee: a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::fatal, "a message"},
	     {"fff: a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::note, counted::MESSAGE_A / 1},
	     {"nnn: MESSAGE_A[ONE]\n"},
	     "plural{501, 1}"},
	    {{"", 0, 0, severity::note, counted::MESSAGE_B / 1000000000},
	     {"nnn: MESSAGE_B[{0}]\n"},
	     "plural{502, 1000000000}"},
	    {{"", 0, 0, severity::note,
	      str(counted::MESSAGE_C / 1, "first", "second", "third", "1")},
	     {"nnn: MESSAGE_C[ONE](first, third)\n"},
	     "formatable{subject:plural{503, 1}, args:{ \"first\"s, "
	     "\"second\"s, \"third\"s, \"1\"s }}"},
	    {{"", 0, 0, severity::note,
	      str(counted::MESSAGE_C / 5, "first", "second", "third", "5")},
	     {"nnn: MESSAGE_C[5](first, third)\n"},
	     "formatable{subject:plural{503, 5}, args:{ \"first\"s, "
	     "\"second\"s, \"third\"s, \"5\"s }}"},
	    {{"", 0, 0, severity::warning,
	      str(counted::MESSAGE_C / 1,
	          counted::ARG_1 / 0,
	          counted::MESSAGE_A / 1,
	          counted::ARG_2 / 2,
	          "1")},
	     {"www: MESSAGE_C[ONE](ARG_1[{0}], ARG_2[{0}])\n"},
	     "formatable{subject:plural{503, 1}, args:{ plural{504, 0}, "
	     "plural{501, 1}, plural{505, 2}, \"1\"s }}"},
	    {{"", 0, 0, severity::warning,
	      str(counted::MESSAGE_C / 1,
	          counted::ARG_1 / 0,
	          counted::MESSAGE_A / 1,
	          counted::ARG_2 / 2,
	          "1")},
	     {"www: MESSAGE_C[ONE](ARG_1[{0}], ARG_2[{0}])\n"},
	     "formatable{subject:plural{503, 1}, args:{ plural{504, 0}, "
	     "plural{501, 1}, plural{505, 2}, \"1\"s }}"},
	    {{"", 0, 0, severity::note, counted::MESSAGE_A / 5, link_type::gcc,
	      false},
	     {"note: message A ({} plurals)\n"},
	     "plural{501, 5}"},
	    {{"", 0, 0, severity::note, str(counted::MESSAGE_A / 5, "5"),
	      link_type::gcc, false},
	     {"note: message A (5 plurals)\n"},
	     "formatable{subject:plural{501, 5}, args:{ \"5\"s }}"},
	    {{"", 0, 0, severity::note, counted::MESSAGE_B / 1, link_type::gcc,
	      false},
	     {"note: message B (plural)\n"},
	     "plural{502, 1}"},
	    {{"", 0, 0, severity::note,
	      str(counted::MESSAGE_C / 0, "first", "second", "third", "zero"),
	      link_type::gcc, false},
	     {"note: message C with first and third (zero plurals)\n"},
	     "formatable{subject:plural{503, 0}, args:{ \"first\"s, "
	     "\"second\"s, \"third\"s, \"zero\"s }}"},
	    {{"", 0, 0, severity::warning,
	      str(counted::MESSAGE_C / 1,
	          counted::ARG_1 / 1,
	          counted::MESSAGE_A / 1,
	          counted::ARG_2 / 1,
	          "uno"),
	      link_type::gcc, false},
	     {"warning: message C with single argument-1 and single argument-2 "
	      "(plural)\n"},
	     "formatable{subject:plural{503, 1}, args:{ plural{504, 1}, "
	     "plural{501, 1}, plural{505, 1}, \"uno\"s }}"},
	    {{"", 0, 0, severity::warning,
	      str("arguments: {0}, {1}, {2}",
	          counted::ARG_1 / 3,
	          counted::MESSAGE_A / 33,
	          counted::ARG_2 / 333)},
	     {"www: arguments: ARG_1[{0}], MESSAGE_A[{0}], ARG_2[{0}]\n"},
	     "formatable{subject:\"arguments: {0}, {1}, {2}\"s, args:{ "
	     "plural{504, 3}, plural{501, 33}, plural{505, 333} }}"},
	};

	static const diagnostic_write path_message[] = {
	    {{"path", 4, 1, severity::verbose, "a message"},
	     {"a message\n"},
	     "\"a message\"s"},
	    {{"path", 5, 2, severity::note, "a message"},
	     {"path:5:2: nnn: a message\n"},
	     "\"a message\"s"},
	    {{"path", 6, 3, severity::warning, "a message"},
	     {"path:6:3: www: a message\n"},
	     "\"a message\"s"},
	    {{"path", 7, 4, severity::error, "a message"},
	     {"path:7:4: eee: a message\n"},
	     "\"a message\"s"},
	    {{"path", 3, 5, severity::fatal, "a message"},
	     {"path:3:5: fff: a message\n"},
	     "\"a message\"s"},
	    {{"path", 2, 6, severity::verbose, "a message", link_type::gcc, false},
	     {"a message\n"},
	     "\"a message\"s"},
	    {{"path", 1, 7, severity::note, "a message", link_type::gcc, false},
	     {"path:1:7: note: a message\n"},
	     "\"a message\"s"},
	    {{"path", 4, 8, severity::warning, "a message", link_type::gcc, false},
	     {"path:4:8: warning: a message\n"},
	     "\"a message\"s"},
	    {{"path", 5, 9, severity::error, "a message", link_type::gcc, false},
	     {"path:5:9: error: a message\n"},
	     "\"a message\"s"},
	    {{"path", 6, 10, severity::fatal, "a message", link_type::gcc, false},
	     {"path:6:10: fatal: a message\n"},
	     "\"a message\"s"},
	};

	INSTANTIATE_TEST_SUITE_P(severities,
	                         diags_write_plural,
	                         ValuesIn(severities));
	INSTANTIATE_TEST_SUITE_P(path, diags_write_plural, ValuesIn(path));
	INSTANTIATE_TEST_SUITE_P(message, diags_write_plural, ValuesIn(message));
	INSTANTIATE_TEST_SUITE_P(path_message,
	                         diags_write_plural,
	                         ValuesIn(path_message));

	static const args<argument> list[] = {
	    {counted::MESSAGE_A / 100, counted::MESSAGE_B / 100},
	    {""s, counted::ARG_2 / 100},
	    {counted::ARG_1 / 100, ""s},
	    {format((counted::MESSAGE_C / 100),
	            "path",
	            counted::ARG_1 / 100,
	            counted::ARG_2 / 100,
	            "100"),
	     counted::MESSAGE_C / 100},
	    {
	        format((counted::MESSAGE_C / 100),
	               "path",
	               counted::ARG_1 / 100,
	               counted::ARG_2 / 100,
	               "100"),
	        format((counted::MESSAGE_C / 100),
	               "path",
	               counted::ARG_1 / 100,
	               counted::ARG_2 / 100,
	               "100"),
	        true,
	    },
	    {format("format: {0}", counted::ARG_1 / 100),
	     format("format: {0}", "argument-1")}};

	INSTANTIATE_TEST_SUITE_P(list, diags_cmp_plural, ValuesIn(list));
}  // namespace diags::testing::plural
