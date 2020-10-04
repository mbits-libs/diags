// Copyright (c) 2015 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <fmt/format.h>
#include <diags/streams.hpp>

namespace diags {
	instream::~instream() = default;

	outstream::~outstream() = default;

	std::size_t outstream::vprint(fmt::string_view format_str,
	                              fmt::format_args args) {
		fmt::memory_buffer buffer;
		fmt::vformat_to(buffer, format_str, args);
		return write(buffer.data(), buffer.size());
	}

	std_outstream::std_outstream(std::FILE* ptr) noexcept : ptr(ptr) {}
	std::size_t std_outstream::write(void const* data,
	                                 std::size_t length) noexcept {
		return std::fwrite(data, 1, length, ptr);
	}

	std_outstream& get_stdout() {
		static std_outstream stdout__{stdout};
		return stdout__;
	}
}  // namespace diags
