// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <diag_write_common.hpp>
#include <support/singular_types.hpp>

namespace diags::testing::singular {
	class diags_write_singular
	    : public DiagsWriteTest<diagnostic_str, strings, translator_type> {};

	TEST_P(diags_write_singular, print) { print_test(); }
	TEST_P(diags_write_singular, dbg) { dbg_test(); }
	TEST_P(diags_write_singular, print_with_source) {
		print_with_source_test();
	}
	TEST_P(diags_write_singular, has_errors) { has_errors_test(); }

	struct diags_cmp_singular : DiagsCmpTest<argument> {};

	TEST_P(diags_cmp_singular, comp) { comp_test(); }

	TEST(diags_write, singular_purecall) {
		strings_mock strings{};
		auto tr = strings.make();

		noenum::string::plural failing{10000, 12345};

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
	     "\"a message\"s",
	     true},
	    {{"", 0, 0, severity::note, "a message"},
	     {"nnn: a message\n"},
	     "\"a message\"s",
	     true},
	    {{"", 0, 0, severity::warning, "a message"},
	     {"www: a message\n"},
	     "\"a message\"s",
	     true},
	    {{"", 0, 0, severity::error, "a message"},
	     {"eee: a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::fatal, "a message"},
	     {"fff: a message\n"},
	     "\"a message\"s"},
	    {{"", 0, 0, severity::note, lng::MESSAGE_A},
	     {"nnn: MESSAGE_A\n"},
	     "singular{501}"},
	    {{"", 0, 0, severity::note, lng::MESSAGE_B},
	     {"nnn: MESSAGE_B\n"},
	     "singular{502}"},
	    {{"", 0, 0, severity::note,
	      str(lng::MESSAGE_C, "first", "second", "third")},
	     {"nnn: MESSAGE_C(first, third)\n"},
	     "formatable{subject:singular{503}, args:{ \"first\"s, "
	     "\"second\"s, \"third\"s }}"},
	    {{"", 0, 0, severity::warning,
	      str(lng::MESSAGE_C, lng::ARG_1, lng::MESSAGE_A, lng::ARG_2)},
	     {"www: MESSAGE_C(ARG_1, ARG_2)\n"},
	     "formatable{subject:singular{503}, args:{ singular{504}, "
	     "singular{501}, singular{505} }}"},
	    {{"", 0, 0, severity::note, lng::MESSAGE_A, link_type::gcc, false},
	     {"note: message A (singular)\n"},
	     "singular{501}"},
	    {{"", 0, 0, severity::note, lng::MESSAGE_B, link_type::gcc, false},
	     {"note: message B (singular)\n"},
	     "singular{502}"},
	    {{"", 0, 0, severity::note,
	      str(lng::MESSAGE_C, "first", "second", "third"), link_type::gcc,
	      false},
	     {"note: message C with first and third (singular)\n"},
	     "formatable{subject:singular{503}, args:{ \"first\"s, "
	     "\"second\"s, \"third\"s }}"},
	    {{"", 0, 0, severity::warning,
	      str(lng::MESSAGE_C, lng::ARG_1, lng::MESSAGE_A, lng::ARG_2),
	      link_type::gcc, false},
	     {"warning: message C with argument-1 and argument-2 "
	      "(singular)\n"},
	     "formatable{subject:singular{503}, args:{ singular{504}, "
	     "singular{501}, singular{505} }}"},
	    {{"", 0, 0, severity::warning,
	      str("arguments: {0}, {1}, {2}",
	          lng::ARG_1,
	          lng::MESSAGE_A,
	          lng::ARG_2)},
	     {"www: arguments: ARG_1, MESSAGE_A, ARG_2\n"},
	     "formatable{subject:\"arguments: {0}, {1}, {2}\"s, args:{ "
	     "singular{504}, singular{501}, singular{505} }}"},
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

	//  1	23457	89.123456789.123456789.1
	// "	words	= value1, value2, value3"sv
	//  123456789.123456789.123456789.123456
	static const diagnostic_write kv[] = {
	    {{"path", 8, 1, severity::note, "key/value", link_type::gcc, false, 2},
	     {"path:8:1: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 2, severity::note, "key/value", link_type::gcc, false, 3},
	     {"path:8:2: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 2, severity::note, "key/value", link_type::gcc, false, 7},
	     {"path:8:2: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 7, severity::note, "key/value", link_type::gcc, false, 8},
	     {"path:8:7: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 8, severity::note, "key/value", link_type::gcc, false, 9},
	     {"path:8:8: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 10, severity::note, "key/value", link_type::gcc, false,
	      16},
	     {"path:8:10: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 18, severity::note, "key/value", link_type::gcc, false,
	      24},
	     {"path:8:18: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 26, severity::note, "key/value", link_type::gcc, false,
	      32},
	     {"path:8:26: note: key/value\n"},
	     "\"key/value\"s"},
	    {{"path", 8, 35, severity::note, "key/value", link_type::gcc, false,
	      36},
	     {"path:8:35: note: key/value\n"},
	     "\"key/value\"s"},
	};

	static const diagnostic_write multiline[] = {
	    {
	        {
	            "path",
	            6,
	            3,
	            severity::warning,
	            "a message",
	            link_type::gcc,
	            false,
	            0,
	            {
	                {"path", 5, 2, severity::note, "an explanation"},
	            },
	        },
	        {
	            "path:6:3: warning: a message\n",
	            "path:5:2: note: an explanation\n",
	        },
	        "\"a message\"s",
	    },
	    {
	        {
	            "path",
	            6,
	            3,
	            severity::warning,
	            "a message",
	            link_type::vc,
	            false,
	            0,
	            {
	                {"path", 5, 2, severity::note, "an explanation"},
	            },
	        },
	        {
	            "path(6,3): warning: a message\n",
	            "    path(5,2): note: an explanation\n",
	        },
	        "\"a message\"s",
	    },
	    {
	        {
	            "path",
	            6,
	            3,
	            severity::warning,
	            "a message",
	            link_type::vc,
	            false,
	            0,
	            {
	                {"path", 5, 2, severity::error, "hidden error"},
	            },
	        },
	        {
	            "path(6,3): warning: a message\n",
	            "    path(5,2): error: hidden error\n",
	        },
	        "\"a message\"s",
	    },
	};

	INSTANTIATE_TEST_SUITE_P(severities,
	                         diags_write_singular,
	                         ValuesIn(severities));
	INSTANTIATE_TEST_SUITE_P(path, diags_write_singular, ValuesIn(path));
	INSTANTIATE_TEST_SUITE_P(message, diags_write_singular, ValuesIn(message));
	INSTANTIATE_TEST_SUITE_P(path_message,
	                         diags_write_singular,
	                         ValuesIn(path_message));
	INSTANTIATE_TEST_SUITE_P(kv, diags_write_singular, ValuesIn(kv));
	INSTANTIATE_TEST_SUITE_P(multiline,
	                         diags_write_singular,
	                         ValuesIn(multiline));

	static const args<argument> list[] = {
	    {lng::MESSAGE_A, lng::MESSAGE_B},
	    {""s, lng::ARG_2},
	    {lng::ARG_1, ""s},
	    {format(lng::MESSAGE_C, "path", lng::ARG_1, lng::ARG_2),
	     lng::MESSAGE_C},
	    {format(lng::MESSAGE_C, "path", lng::ARG_1, lng::ARG_2),
	     format(lng::MESSAGE_C, "path", lng::ARG_1, lng::ARG_2), true},
	    {format("format: {0}", lng::ARG_1),
	     format("format: {0}", "argument-1")}};

	INSTANTIATE_TEST_SUITE_P(list, diags_cmp_singular, ValuesIn(list));
}  // namespace diags::testing::singular
