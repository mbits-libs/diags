// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <support/printer.hpp>

namespace diags::testing {
	char const* name(severity sev) {
		switch (sev) {
#define NAME(x)       \
	case severity::x: \
		return "severity::" #x;
			NAME(verbose);
			NAME(note);
			NAME(warning);
			NAME(error);
			NAME(fatal);
#undef NAME
		};
		return "!!!";
	}

	void info(std::ostream& o, noenum::string::argument const& arg) {
		std::visit(
		    [&](auto const& arg) {
			    if constexpr (std::is_same_v<decltype(arg),
			                                 std::string const&>) {
				    o << "\"" << arg << "\"";
			    } else if constexpr (std::is_same_v<
			                             decltype(arg),
			                             noenum::string::formatable const&>) {
				    info(o, arg);
			    } else if constexpr (std::is_same_v<
			                             decltype(arg),
			                             noenum::string::singular const&>) {
				    o << '[' << arg.id << ']';
			    } else {
				    o << '[' << arg.id << '/' << arg.count << ']';
			    }
		    },
		    static_cast<noenum::string::argument::base_class const&>(arg));
	}

	void info(std::ostream& o, noenum::string::formatable const& arg) {
		std::visit(
		    [&](auto const& arg) {
			    if constexpr (std::is_same_v<decltype(arg),
			                                 std::string const&>) {
				    o << "\"" << arg << "\"";
			    } else if constexpr (std::is_same_v<
			                             decltype(arg),
			                             noenum::string::singular const&>) {
				    o << '[' << arg.id << ']';
			    } else {
				    o << "[" << arg.id << "/" << arg.count << "]";
			    }
		    },
		    arg.subject());

		if (arg.arguments().empty()) return;

		o << "(";
		auto first = true;
		for (auto const& sub : arg.arguments()) {
			if (first)
				first = false;
			else
				o << ",";
			info(o, sub);
		}
		o << ")";
	}

	void info(std::ostream& o, diagnostic const& diag) {
		o << '(' << diag.start().line << ':' << diag.start().column << " - "
		  << diag.stop().line << ':' << diag.stop().column << ")["
		  << name(diag.severity()) << "] << ";
		info(o, diag.message());
	}
}  // namespace diags::testing
