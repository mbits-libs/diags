// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/location.hpp>
#include <diags/streams.hpp>

namespace diags {
	class source_code : public instream {
		friend class sources;
		class source_view;
		std::shared_ptr<source_view> view_;
		size_t position_{0};
		source_code(std::shared_ptr<source_view> view) : view_{view} {}

	public:
		source_code() = default;
		~source_code();
		source_code(source_code const&) = delete;
		source_code& operator=(source_code const&) = delete;
		source_code(source_code&&) = default;
		source_code& operator=(source_code&&) = default;

		[[nodiscard]] bool valid() const noexcept;

		std::size_t read(void* buffer, std::size_t length) noexcept final;
		bool eof() const noexcept final;
		std::byte peek() noexcept final;
		std::size_t tell() const noexcept final;
		std::size_t seek(std::size_t) noexcept final;
		std::vector<std::byte> const& data() const noexcept;
		std::string_view line(unsigned no) noexcept;

		location position(unsigned line = 0, unsigned column = 0);
	};
}  // namespace diags
