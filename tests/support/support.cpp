// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <support/printer.hpp>
#include <support/support.hpp>

namespace diags {
	void PrintTo(severity sev, std::ostream* o) { *o << testing::name(sev); }

	namespace noenum::string {
		void PrintTo(argument const& arg, std::ostream* o) {
			testing::info(*o, arg);
		}

		void PrintTo(message const& arg, std::ostream* o) {
			testing::info(*o, arg);
			*o << " :: [" << arg.dbg() << ']';
		}
	}  // namespace noenum::string
}  // namespace diags
