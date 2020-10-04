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
			std::string path;
			std::shared_ptr<source_code::source_view> contents{};
			unsigned key{};
			info(std::string path, unsigned key)
			    : path(std::move(path)), key(key) {}
		};

		using shared_info = std::shared_ptr<info>;
		std::map<std::string, shared_info, std::less<>> files_{};
		std::map<unsigned, shared_info> reverse_{};

		unsigned current_value_ = 0;

		std::vector<diagnostic> set_{};

		template <typename Key>
		shared_info lookup(Key const& path);

		template <typename Key>
		shared_info lookup(Key const& path) const;

	public:
		std::string_view filename(location const&) const;
		source_code open(std::string const& path, char const* mode = "r");
		source_code source(std::string_view const& path);
		source_code source(std::string_view const& path) const;
		void set_contents(std::string const& path, std::vector<std::byte> data);
		void set_contents(std::string const& path, std::string_view data);

		void push_back(diagnostic diag);

		std::vector<diagnostic> const& diagnostic_set() const noexcept {
			return set_;
		}

		bool has_errors() const noexcept;
	};
}  // namespace diags
