// Copyright (c) 2015 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <diags/streams.hpp>

namespace diags::fs {
	FILE* file::fopen(path file, char const* mode) noexcept {
		file.make_preferred();
#if defined WIN32 || defined _WIN32
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
		wchar_t buff[20];
		auto len = std::min(mode ? strlen(mode) : 0, sizeof(buff) - 1);

		auto dst = buff;
		auto end = dst + len;
		while (dst != end)
			*dst++ = *mode++;
		*dst = 0;

		return ::_wfopen(file.native().c_str(), buff);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#else  // WIN32 || _WIN32
		return std::fopen(file.string().c_str(), mode);
#endif
	}

	std::vector<std::byte> file::read() const noexcept {
		std::vector<std::byte> out;
		if (!*this) return out;
		std::byte buffer[1024];

		while (true) {
			auto ret = std::fread(buffer, 1, sizeof(buffer), get());
			if (!ret) {
				if (!std::feof(get())) out.clear();
				break;
			}
			out.insert(end(out), buffer, buffer + ret);
		}

		return out;
	}

	size_t file::load(void* buffer, size_t length) const noexcept {
		return std::fread(buffer, 1, length, get());
	}

	size_t file::store(void const* buffer, size_t length) const noexcept {
		return std::fwrite(buffer, 1, length, get());
	}

	file fopen(path const& file, char const* mode) noexcept {
		return {file, mode};
	}
}  // namespace diags::fs
