// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <diags/diagnostic.hpp>
#include <iostream>

namespace diags::testing {
	char const* name(severity sev);
	void info(std::ostream& o, noenum::string::argument const& arg);
	void info(std::ostream& o, noenum::string::formatable const& arg);
	void info(std::ostream& o, diagnostic const& arg);
}  // namespace diags::testing
