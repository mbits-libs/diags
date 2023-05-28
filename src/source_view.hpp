// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <algorithm>
#include <diags/source_code.hpp>
#include <diags/sources.hpp>

namespace diags {
	class source_code::source_view {
		sources::info* item_;
		fs::file file_;
		std::vector<std::byte> content_;
		std::vector<std::size_t> endlines_;
		bool eof_{false};

	public:
		source_view(sources::info* const& item) : item_{item} {}

		void open(char const* mode) { file_.open(item_->path, mode); }

		unsigned key() const noexcept { return item_->key; }

		void ensure(size_t size) {
			if (!valid()) eof_ = true;

			std::byte buffer[8192];
			while (!eof_ && content_.size() < size) {
				auto read = file_.load(buffer, sizeof(buffer));
				if (read)
					content_.insert(content_.end(), buffer, buffer + read);

				if (file_.feof()) eof_ = true;
			}
		}

		std::vector<std::byte> const& data() noexcept {
			std::byte buffer[8192];
			while (!eof_) {
				auto read = file_.load(buffer, sizeof(buffer));
				if (read)
					content_.insert(content_.end(), buffer, buffer + read);

				if (file_.feof()) eof_ = true;
			}

			return content_;
		}

		void data(std::vector<std::byte>&& bytes) noexcept {
			eof_ = true;
			file_.close();
			content_ = std::move(bytes);
			endlines_.clear();
		}

		std::pair<std::string_view, std::size_t> line(unsigned no) noexcept {
			constexpr auto endline = std::byte{'\n'};

			if (!no) return {};
			--no;

			while (no >= endlines_.size()) {
				auto current = endlines_.empty() ? 0u : (endlines_.back() + 1);
				if (current < content_.size()) {
					auto data = content_.data() + current;
					auto end = content_.data() + content_.size();
					auto it = std::find(data, end, endline);

					if (it != end) {
						endlines_.push_back(
						    static_cast<size_t>(it - content_.data()));
						continue;
					}
				}

				if (eof_) {
					if (endlines_.empty() ||
					    endlines_.back() != content_.size())
						endlines_.push_back(content_.size());
					break;
				}

				ensure(content_.size() + 1024);
			}

			if (no < endlines_.size()) {
				auto data = reinterpret_cast<char const*>(content_.data());
				if (!no) return {{data, endlines_[0]}, endlines_[0]};
				const auto prev = endlines_[no - 1];
				return {{data + prev + 1, endlines_[no] - prev - 1},
				        endlines_[no]};
			}

			return {};
		}

		bool valid() const noexcept { return file_ || eof_; }

		bool eof(size_t at) const noexcept {
			return eof_ && (at >= content_.size());
		}

		std::byte peek(size_t at) noexcept {
			ensure(at + 1);
			if (at >= content_.size()) return std::byte{0};
			return content_[at];
		}

		size_t read(size_t at, void* buffer, size_t length) {
			ensure(at + length);
			auto rest = content_.size();
			if (rest > at)
				rest -= at;
			else
				rest = 0;

			if (rest > length) rest = length;

			if (rest) std::memcpy(buffer, content_.data() + at, rest);
			return rest;
		}

		size_t seek(size_t at) {
			ensure(at);
			if (at < content_.size()) return at;
			return content_.size();
		}
	};
}  // namespace diags
