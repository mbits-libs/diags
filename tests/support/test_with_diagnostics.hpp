// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/diagnostic.hpp>
#include <diags/location.hpp>
#include <string_view>
#include <support/printer.hpp>

namespace diags::testing {
	template <typename DiagnosticStr>
	struct expected_diagnostic {
		char const* filename{""};
		unsigned line{0};
		unsigned column{0};
		severity sev{severity::verbose};
		DiagnosticStr message{};
		link_type link{link_type::gcc};
		bool use_alt_tr{true};
		unsigned column_end{0};
		std::vector<expected_diagnostic<DiagnosticStr>> subs{};
	};

	template <typename DiagnosticStr>
	inline std::ostream& operator<<(
	    std::ostream& o,
	    expected_diagnostic<DiagnosticStr> const& diag) {
		o << "{\"" << diag.filename << "\"," << diag.line << ',' << diag.column;
		if (diag.column_end) o << ':' << diag.column_end;
		return o << ',' << name(diag.sev) << "," << diag.message << ","
		         << (diag.link == link_type::gcc ? "gcc" : "vc") << ","
		         << (diag.use_alt_tr ? "alt" : "tr") << '}';
	}

	struct UnexpectedDiags {
		std::vector<diagnostic> const& diags;
		size_t skip;
		UnexpectedDiags(std::vector<diagnostic> const& diags, size_t skip)
		    : diags{diags}, skip{skip} {}

		friend std::ostream& operator<<(std::ostream& o,
		                                UnexpectedDiags const& data) {
			o << "Missing diagnostics:\n";
			for (size_t i = data.skip; i < data.diags.size(); ++i) {
				o << "  ";
				info(o, data.diags[i]);
				o << '\n';
			}
			return o;
		}
	};

	template <typename ActualTest>
	struct TestWithDiagnostics : ActualTest {
	protected:
		void ExpectDiagsEq(std::vector<diagnostic> const& expected,
		                   std::vector<diagnostic> const& actual,
		                   unsigned src) {
			EXPECT_GE(expected.size(), actual.size())
			    << UnexpectedDiags{actual, expected.size()};
			EXPECT_GE(actual.size(), expected.size())
			    << UnexpectedDiags{expected, actual.size()};

			auto exp_it = begin(expected);
			auto act_it = begin(actual);
			auto size = std::min(expected.size(), actual.size());
			for (decltype(size) i = 0; i < size; ++i, ++exp_it, ++act_it) {
				auto const& exp_msg = *exp_it;
				auto const& act_msg = *act_it;
				if (exp_msg.start().token)
					EXPECT_EQ(exp_msg.start().token, act_msg.start().token);
				else
					EXPECT_EQ(src, act_msg.start().token);
				EXPECT_EQ(exp_msg.severity(), act_msg.severity());
				EXPECT_EQ(exp_msg.message(), act_msg.message());
				ExpectDiagsEq(exp_msg.children(), act_msg.children(), src);
			}
		}
	};
}  // namespace diags::testing
