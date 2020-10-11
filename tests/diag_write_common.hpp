// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <diags/printer.hpp>
#include <diags/sources.hpp>
#include <numeric>
#include <support/outstrstream.hpp>
#include <support/test_with_diagnostics.hpp>

namespace diags::noenum::string {
	void PrintTo(argument const& arg, std::ostream* o);
}

namespace diags::string {
	template <typename Enum>
	inline static std::ostream& operator<<(
	    std::ostream& out,
	    argument_singulars<Enum> const& arg) {
		noenum::string::argument const& slice = arg;
		PrintTo(slice, &out);
		return out;
	}

	template <typename Enum>
	inline static std::ostream& operator<<(std::ostream& out,
	                                       argument_plurals<Enum> const& arg) {
		noenum::string::argument const& slice = arg;
		PrintTo(slice, &out);
		return out;
	}

	template <typename SEnum, typename PEnum>
	inline static std::ostream& operator<<(
	    std::ostream& out,
	    argument_singulars_with_plurals<SEnum, PEnum> const& arg) {
		noenum::string::argument const& slice = arg;
		PrintTo(slice, &out);
		return out;
	}
}  // namespace diags::string

namespace diags::testing {
	using namespace ::std::literals;
	using ::testing::TestWithParam;
	using ::testing::ValuesIn;

	template <typename Argument>
	struct args {
		Argument left{};
		Argument right{};
		bool should_be_equal{false};
	};

	template <typename DiagnosticStr>
	struct diagnostic_write {
		expected_diagnostic<DiagnosticStr> value;
		std::initializer_list<char const*> expected;
		std::string_view debug{"\"\"s"};
		bool use_view_content{false};
	};

	template <typename DiagnosticStr>
	std::ostream& operator<<(std::ostream& o,
	                         diagnostic_write<DiagnosticStr> const& diag) {
		return o << diag.value;
	}

	template <typename DiagnosticStr>
	diagnostic conv(sources& host,
	                expected_diagnostic<DiagnosticStr> const& exp) {
		const auto loc =
		    host.source(exp.filename).position(exp.line, exp.column);
		auto end = exp.column_end ? loc.moved_to(loc.line, exp.column_end)
		                          : location{};

		auto d = loc - end[exp.sev] << exp.message.arg();
		d.children_reserve(exp.subs.size());
		for (auto const& child : exp.subs)
			d.with(conv(host, child));
		return d;
	}

	inline std::pair<std::string_view, std::string_view> color_select(
	    severity sev,
	    color color_type) {
		static auto const color_potentially_allowed = get_stdout().is_tty();
		bool color_allowed = color_type == color::automatic
		                         ? color_potentially_allowed
		                         : color_type == color::always;
		using color_info =
		    std::pair<diags::severity,
		              std::pair<std::string_view, std::string_view>>;
		static constexpr std::string_view reset = "\x1b[0m";
		static constexpr color_info colors[] = {
		    {severity::verbose, {}},
		    {severity::note, {"\x1b[1;36m", reset}},
		    {severity::warning, {"\x1b[1;35m", reset}},
		    {severity::error, {"\x1b[1;31m", reset}},
		};

		static constexpr auto colors_size = sizeof(colors) / sizeof(colors[0]);

		if (!color_allowed) return colors[0].second;
		for (auto [cmp, clr] : colors) {
			if (cmp == sev) return clr;
		}
		return colors[colors_size - 1].second;
	}

	template <typename It, typename DiagnosticStr, size_t length>
	void output(std::string& expected,
	            It& curr,
	            It end,
	            expected_diagnostic<DiagnosticStr> const& value,
	            std::string_view const (&text)[length]) {
		if (curr != end) expected.append(*curr++);

		auto [color_start, color_reset] =
		    color_select(value.sev, value.color_type);

		if (value.line) {
			struct index {
				size_t raw, mapped{0};
			};

			auto line = text[value.line - 1];
			index col{value.column};
			index col_end{value.column_end};

			size_t len = 0;
			size_t pos = 0;

			for (auto c : line) {
				++len;
				++pos;
				if (col.raw == pos) {
					col.mapped = len;
					expected.append(color_start);
				}
				if (col_end.raw == pos) {
					col_end.mapped = len;
					expected.append(color_reset);
				}
				if (c == '\t') {
					expected.push_back(' ');
					while (len % diagnostic::tab_size) {
						++len;
						expected.push_back(' ');
					}
				} else {
					expected.push_back(c);
				}

				if (!col_end.raw && col.raw == pos) {
					expected.append(color_reset);
				}
			}
			if (col.raw && !col.mapped) col.mapped = len + (col.raw - pos);
			if (col_end.raw && !col_end.mapped)
				col_end.mapped = len + (col_end.raw - pos);

			expected.push_back('\n');
			if (value.column) {
				for (auto i = 0u; i < (col.mapped - 1); ++i)
					expected.push_back(' ');
				expected.append(color_start);
				expected.push_back('^');
				if (value.column_end) {
					for (auto i = col.mapped; i < (col_end.mapped - 1); ++i)
						expected.push_back('~');
				}
				expected.append(color_reset);
				expected.push_back('\n');
			}
		}

		for (auto const& sub : value.subs)
			output(expected, curr, end, sub, text);
	}

	template <typename DiagnosticStr, typename Strings, typename TranslatorType>
	class DiagsWriteTest
	    : public TestWithParam<diagnostic_write<DiagnosticStr>> {
	protected:
		void print_test() {
			auto [value, exp_str, _, _2] = this->GetParam();

			Strings strings_source;
			auto tr = strings_source.template choose<TranslatorType>(
			    value.use_alt_tr);

			outstrstream actual{};
			sources host{};
			host.set_printer<printer>(&actual, tr, value.color_type,
			                          value.link);
			host.print_diagnostic(conv(host, value));

			std::string expected;
			expected.reserve(std::accumulate(begin(exp_str), end(exp_str),
			                                 size_t{},
			                                 [](size_t acc, auto const& view) {
				                                 return acc + strlen(view);
			                                 }));

			for (auto const& line : exp_str)
				expected.append(line);

			ASSERT_EQ(expected, actual.contents);
		}

		void dbg_test() {
			auto [value, _, expected, _2] = this->GetParam();
			auto const actual = value.message.arg().dbg();
			ASSERT_EQ(expected, actual);
		}

		void print_with_source_test() {
			static constexpr const std::string_view text[] = {
			    "one012345678901234567890123456789012345678901234567890123456789"sv,
			    "two012345678901234567890123456789012345678901234567890123456789"sv,
			    "three012345678901234567890123456789012345678901234567890123456789"sv,
			    "four012345678901234567890123456789012345678901234567890123456789"sv,
			    "five012345678901234567890123456789012345678901234567890123456789"sv,
			    "six012345678901234567890123456789012345678901234567890123456789"sv,
			    "seven012345678901234567890123456789012345678901234567890123456789"sv,
			    "\twords\t= value1, value2, value3"sv};

			auto [value, exp_str, _, use_view_content] = this->GetParam();

			Strings strings_source;
			auto tr = strings_source.template choose<TranslatorType>(
			    value.use_alt_tr);

			outstrstream actual{};
			sources host{};

			auto const text_length =
			    std::accumulate(std::begin(text), std::end(text), size_t{},
			                    [](size_t acc, auto const& view) {
				                    return acc + view.length() + 1;
			                    });

			if (use_view_content) {
				std::string contents;
				contents.reserve(text_length);
				for (auto const& line : text) {
					contents.append(line);
					contents.push_back('\n');
				}
				host.set_contents(value.filename, contents);
			} else {
				std::vector<std::byte> contents;
				contents.reserve(text_length);
				for (auto const& line : text) {
					auto b = reinterpret_cast<std::byte const*>(line.data());
					auto e = b + line.length();
					contents.insert(end(contents), b, e);
					contents.push_back(std::byte('\n'));
				}
				host.set_contents(value.filename, contents);
			}

			host.set_printer<printer>(&actual, tr, value.color_type,
			                          value.link);
			host.push_back(conv(host, value));
			host.print_diagnostics();

			std::string expected;
			{
				auto b = begin(exp_str);
				output(expected, b, end(exp_str), value, text);
			}

			ASSERT_EQ(expected, actual.contents);
		}

		void has_errors_test() {
			auto [value, ignore, _, _2] = this->GetParam();

			sources host;
			host.push_back(conv(host, value));

			auto expected = value.sev > severity::warning;
			ASSERT_EQ(expected, host.has_errors()) << name(value.sev);
		}
	};

	template <typename ArgumentType>
	class DiagsCmpTest : public TestWithParam<args<ArgumentType>> {
	public:
		void comp_test() {
			auto [lhs, rhs, should_be_equal] = this->GetParam();
			if (should_be_equal) {
				EXPECT_EQ(lhs, rhs)
				    << "LHS: " << lhs.dbg() << "\nRHS: " << rhs.dbg();
			} else {
				EXPECT_NE(lhs, rhs)
				    << "LHS: " << lhs.dbg() << "\nRHS: " << rhs.dbg();
			}
		}
	};
}  // namespace diags::testing
