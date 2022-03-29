// Copyright (c) 2015 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <fmt/format.h>
#include <diags/streams.hpp>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#define _isatty(FD) isatty(FD)
#define _fileno(OBJ) fileno(OBJ)
#endif

namespace diags {
	bool is_terminal(FILE* out) noexcept {
#ifndef _WIN32
		char const* term = getenv("TERM");
#endif
		return (_isatty(_fileno(out)) != 0)
#ifndef _WIN32
		       && term && strcmp(term, "dumb") != 0;
#endif
		;
	}

	instream::~instream() = default;

	outstream::~outstream() = default;

	bool outstream::is_tty() const noexcept { return false; }

	std::size_t outstream::vprint(fmt::string_view format_str,
	                              fmt::format_args args) {
		fmt::memory_buffer buffer;
		fmt::vformat_to(std::back_inserter(buffer), format_str, args);
		return write(buffer.data(), buffer.size());
	}

	std_outstream::std_outstream(std::FILE* ptr) noexcept : ptr(ptr) {}

	bool std_outstream::is_tty() const noexcept { return is_terminal(ptr); }

	std::size_t std_outstream::write(void const* data,
	                                 std::size_t length) noexcept {
		return std::fwrite(data, 1, length, ptr);
	}

	std_outstream& get_stdout() {
		static std_outstream stdout__{stdout};
		return stdout__;
	}

	foutstream::foutstream(fs::file file) noexcept : file(std::move(file)) {}

	std::size_t foutstream::write(void const* data,
	                              std::size_t length) noexcept {
		return file.store(data, length);
	}
}  // namespace diags
