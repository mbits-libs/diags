// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <diags/source_code.hpp>
#include "source_view.hpp"

namespace diags {

	source_code::~source_code() = default;

	bool source_code::valid() const noexcept { return view_ && view_->valid(); }

	std::size_t source_code::read(void* buffer, std::size_t length) noexcept {
		auto read = view_->read(position_, buffer, length);
		position_ += read;
		return read;
	}

	bool source_code::eof() const noexcept { return view_->eof(position_); }

	std::byte source_code::peek() noexcept { return view_->peek(position_); }

	std::size_t source_code::tell() const noexcept { return position_; }

	std::size_t source_code::seek(std::size_t pos) noexcept {
		position_ = view_->seek(pos);
		return position_;
	}

	std::vector<std::byte> const& source_code::data() const noexcept {
		return view_->data();
	}

	std::string_view source_code::line(unsigned no) noexcept {
		auto [line, pos] = view_->line(no);
		if (pos) position_ = pos;
		return line;
	}

	location source_code::position(unsigned line, unsigned column) {
		return {view_ ? view_->key() : 0u, line, column};
	}
}  // namespace diags
