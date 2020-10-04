// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/streams.hpp>

namespace diags::testing {
	struct outstrstream : outstream {
		std::string contents;

		using outstream::write;
		std::size_t write(void const* data, std::size_t length) noexcept final {
			auto b = static_cast<char const*>(data);
			auto e = b + length;
			auto size = contents.size();
			contents.insert(end(contents), b, e);
			return contents.size() - size;
		}
	};
}  // namespace diags::testing
