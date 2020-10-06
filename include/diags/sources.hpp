// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/diagnostic.hpp>
#include <diags/source_code.hpp>
#include <diags/streams.hpp>
#include <map>

#define SINGLE_MAP

namespace diags {
	class sources {
		friend class source_code::source_view;

		struct info {
			fs::path path;
			std::shared_ptr<source_code::source_view> contents{};
			unsigned key{};
			info(fs::path path, unsigned key)
			    : path(std::move(path)), key(key) {}
		};

		using shared_info = std::shared_ptr<info>;
		std::map<fs::path, shared_info, std::less<>> files_{};
		std::map<unsigned, shared_info> reverse_{};

		unsigned current_value_ = 0;

		std::vector<diagnostic> set_{};

		shared_info lookup(fs::path const& path);
		shared_info lookup(fs::path const& path) const;

	public:
		fs::path const& filename(location const&) const;
		source_code open(fs::path const& path, char const* mode = "r");
		source_code source(fs::path const& path);
		source_code source(fs::path const& path) const;
		void set_contents(fs::path const& path, std::vector<std::byte> data);
		void set_contents(fs::path const& path, std::string_view data);

		void push_back(diagnostic diag);

		std::vector<diagnostic> const& diagnostic_set() const noexcept {
			return set_;
		}

		bool has_errors() const noexcept;
	};
}  // namespace diags
