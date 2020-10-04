// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <gtest/gtest.h>
#include <diags/sources.hpp>
#include <diags/string.hpp>
#include <iostream>

namespace diags {
	void PrintTo(severity sev, std::ostream* o);
	namespace noenum::string {
		void PrintTo(argument const& arg, std::ostream* o);
		void PrintTo(message const& arg, std::ostream* o);
	}  // namespace noenum::string
}  // namespace diags
