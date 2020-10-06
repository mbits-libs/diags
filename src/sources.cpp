// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <diags/sources.hpp>
#include "source_view.hpp"

namespace diags {
	fs::path const& sources::filename(location const& loc) const {
		auto it = reverse_.find(loc.token);
		if (it == end(reverse_)) {
			static fs::path empty{};
			return empty;
		}

		return it->second->path;
	}

	source_code sources::open(fs::path const& path, char const* mode) {
		auto item = lookup(path);
		if (!item->contents) {
			(item->contents =
			     std::make_shared<source_code::source_view>(item.get()))
			    ->open(mode);
		}
		return item->contents;
	}

	source_code sources::source(fs::path const& path) {
		auto item = lookup(path);
		if (!item->contents)
			item->contents =
			    std::make_shared<source_code::source_view>(item.get());

		return item->contents;
	}

	source_code sources::source(fs::path const& path) const {
		auto item = lookup(path);
		return item ? item->contents : source_code{};
	}

	void sources::set_contents(fs::path const& path,
	                           std::vector<std::byte> data) {
		auto item = lookup(path);
		if (item) {
			(item->contents =
			     std::make_shared<source_code::source_view>(item.get()))
			    ->data(std::move(data));
		}
	}

	void sources::set_contents(fs::path const& path, std::string_view data) {
		auto item = lookup(path);
		if (item) {
			std::vector<std::byte> bytes(data.length());
			memcpy(bytes.data(), data.data(), data.length());
			(item->contents =
			     std::make_shared<source_code::source_view>(item.get()))
			    ->data(std::move(bytes));
		}
	}

	void sources::push_back(diagnostic diag) {
		set_.push_back(std::move(diag));
	}

	void sources::print_diagnostic(diagnostic const& diag) const {
		prn_->print(diag, *this);
	}

	void sources::print_diagnostics(std::vector<diagnostic> const& diags) const {
		prn_->print(diags, *this);
	}

	bool sources::has_errors() const noexcept {
		for (auto const& diag : set_) {
			if (diag.severity() > severity::warning) return true;
		}
		return false;
	}

	sources::shared_info sources::lookup(fs::path const& path) {
		auto it = files_.lower_bound(path);
		if (it == end(files_) || it->first != path) {
			auto current = ++current_value_;

			auto info = std::make_shared<sources::info>(path, current);

			it = files_.insert(it, {path, info});
			reverse_[current] = info;
		}

		return it->second;
	}

	sources::shared_info sources::lookup(fs::path const& path) const {
		auto it = files_.lower_bound(path);
		if (it == end(files_) || it->first != path) return {};

		return it->second;
	}
}  // namespace diags
