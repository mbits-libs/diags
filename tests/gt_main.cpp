/*
    This file is taken almost verbatim from gmock_main.cc. Therefore
    copyright claim is retained from original file.
*/

// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)

/*
    Additionally, any changes to the source code are covered by:
*/

/*
 * Copyright (C) 2016 midnightBITS
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <filesystem>
#include <iostream>
#include <string_view>
#include <support/default_data_path.hpp>
#include "gtest/gtest.h"

std::filesystem::path TESTING_data_path = default_data_path;

namespace {
	template <typename String>
	static bool ParseStringFlag(std::string_view str,
	                            std::string_view flag,
	                            String* value) {
		if (str.size() < 2 || str.substr(0, 2) != "--") return false;
		str = str.substr(2);

		if (str.size() < flag.size() || str.substr(0, flag.size()) != flag)
			return false;
		str = str.substr(flag.size());

		if (str.empty() || str.front() != '=') return false;
		str = str.substr(1);

		if (str.empty()) return false;
		value->assign(str);
		return true;
	}
}  // namespace

#if GTEST_OS_WINDOWS
#include <tchar.h>
int _tmain(int argc, TCHAR** argv)
#else
int main(int argc, char** argv)
#endif
{
	std::cout << "Running main() from gt_main.cpp\n";
	testing::InitGoogleTest(&argc, argv);
	if (argc > 0) {
		for (int i = 1; i != argc; i++) {
			std::string value;
			if (ParseStringFlag(argv[i], "data_path", &value) &&
			    !value.empty()) {
				printf("Note: data used from path: %s\n", value.c_str());
				TESTING_data_path = value;
			}
		}
	}

	return RUN_ALL_TESTS();
}