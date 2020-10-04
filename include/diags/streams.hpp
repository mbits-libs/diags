// Copyright (c) 2015 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <fmt/core.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>

namespace diags {
	namespace fs {
		using namespace std::filesystem;
		using std::error_code;

		struct fcloser {
			void operator()(FILE* f) { std::fclose(f); }
		};
		class file : private std::unique_ptr<FILE, fcloser> {
			using parent_t = std::unique_ptr<FILE, fcloser>;
			static FILE* fopen(path fname, char const* mode) noexcept;

		public:
			file() = default;
			~file() = default;
			file(file const&) = delete;
			file& operator=(file const&) = delete;
			file(file&&) = default;
			file& operator=(file&&) = default;

			explicit file(path const& fname) noexcept : file(fname, "r") {}
			file(path const& fname, char const* mode) noexcept
			    : parent_t(fopen(fname, mode)) {}

			using parent_t::operator bool;

			void close() noexcept { reset(); }
			void open(path const& fname, char const* mode = "r") noexcept {
				reset(fopen(fname, mode));
			}

			std::vector<std::byte> read() const noexcept;
			size_t load(void* buffer, size_t length) const noexcept;
			size_t store(void const* buffer, size_t length) const noexcept;
			bool feof() const noexcept { return std::feof(get()); }
		};

		file fopen(path const& file, char const* mode = "r") noexcept;
	}  // namespace fs

	struct instream {
		instream() = default;
		instream(instream const&) = delete;
		instream(instream&&) = default;
		instream& operator=(instream const&) = delete;
		instream& operator=(instream&&) = default;

		virtual ~instream();
		virtual std::size_t read(void* buffer, std::size_t length) noexcept = 0;
		virtual bool eof() const noexcept = 0;
		virtual std::byte peek() noexcept = 0;
		virtual std::size_t tell() const noexcept = 0;
		virtual std::size_t seek(std::size_t) noexcept = 0;
	};

	struct outstream {
		outstream() = default;
		outstream(outstream const&) = delete;
		outstream(outstream&&) = default;
		outstream& operator=(outstream const&) = delete;
		outstream& operator=(outstream&&) = default;

		virtual ~outstream();
		virtual std::size_t write(void const* buffer,
		                          std::size_t length) noexcept = 0;

		std::size_t write(std::string const& s) noexcept {
			return write(s.c_str(), s.length());
		}

		template <typename T>
		std::size_t write(T const& obj) noexcept {
			// will yield 1 on success or 0 on error
			return write(&obj, sizeof(obj)) / sizeof(obj);
		}

		template <typename... Args>
		inline std::size_t print(fmt::string_view format_str,
		                         Args const&... args) {
			fmt::format_arg_store<fmt::format_context, Args...> as(args...);
			return vprint(format_str, as);
		}

		template <typename... Args>
		inline std::size_t fmt(::fmt::string_view format_str,
		                       Args const&... args) {
			::fmt::format_arg_store<::fmt::format_context, Args...> as(args...);
			return vprint(format_str, as);
		}

	private:
		std::size_t vprint(fmt::string_view format_str, fmt::format_args args);
	};

	class std_outstream : public outstream {
		std::FILE* ptr;

	public:
		std_outstream(std::FILE* ptr) noexcept;
		using outstream::write;
		std::size_t write(void const* data, std::size_t length) noexcept final;
	};

	std_outstream& get_stdout();

	class foutstream : public outstream {
		fs::file file;

	public:
		foutstream(fs::file file) noexcept : file(std::move(file)) {}
		using outstream::write;
		std::size_t write(void const* data, std::size_t length) noexcept final {
			return file.store(data, length);
		}
	};
}  // namespace diags
