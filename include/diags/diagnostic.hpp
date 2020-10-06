// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <diags/location.hpp>
#include <diags/string.hpp>
#include <diags/streams.hpp>
#include <vector>

namespace diags {
	class sources;
	class source_code;

	enum class link_type {
		gcc,
		vc,
#ifdef _WIN32
		native = vc
#else
		native = gcc
#endif
	};

	class diagnostic {
	public:
		diagnostic() = default;
		diagnostic(location const& start,
		           location const& stop,
		           severity severity,
		           noenum::string::message&& message,
		           std::vector<diagnostic>&& children = {})
		    : start_{start}
		    , stop_{stop}
		    , severity_{severity}
		    , message_{std::move(message)}
		    , children_{std::move(children)} {}

		std::vector<std::string> format(
		    sources const& host,
		    translator const& tr,
		    link_type links = link_type::native) const {
			auto const lines = calc_lines(host);

			std::vector<std::string> result{};
			result.reserve(lines);

			format(result, host, tr, links, 0);
			return result;
		}

		diagnostic& with(diagnostic sub) {
			children_.emplace_back(std::move(sub));
			return *this;
		}

		location const& start() const noexcept { return start_; }
		location const& stop() const noexcept { return stop_; }
		diags::severity severity() const noexcept { return severity_; }
		noenum::string::message const& message() const noexcept {
			return message_;
		};
		noenum::string::message& message() noexcept { return message_; };
		std::vector<diagnostic> const& children() const noexcept {
			return children_;
		};
		void children_reserve(size_t new_capacity) noexcept {
			children_.reserve(new_capacity);
		};

		static constexpr size_t tab_size = 3;

	private:
		struct line_indices {
			std::string line;
			size_t start_column;
			size_t stop_column;
		};

		size_t calc_lines(sources const& host) const noexcept;

		void format(std::vector<std::string>& output,
		            sources const& host,
		            translator const& tr,
		            link_type links,
		            size_t depth) const;

		std::string message_line(fs::path const& filename,
		                         translator const& tr,
		                         link_type links,
		                         size_t depth) const;

		std::pair<std::string, std::string> hint_lines(
		    source_code& source) const;

		line_indices prepare(std::string_view,
		                     size_t start_col,
		                     size_t stop_col) const noexcept;

		location start_{};
		location stop_{};
		diags::severity severity_{diags::severity::note};
		noenum::string::message message_{};
		std::vector<diagnostic> children_{};
	};

	inline diagnostic operator<<(location_severity const& loc_sev,
	                             noenum::string::message msg) {
		return {loc_sev.loc, {}, loc_sev.sev, std::move(msg)};
	}

	inline diagnostic operator<<(location_severity const& loc_sev,
	                             char const* msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::string_tag{}, msg);
	}

	inline diagnostic operator<<(location_severity const& loc_sev,
	                             std::string const& msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::string_tag{}, std::move(msg));
	}

	inline diagnostic operator<<(location_severity const& loc_sev,
	                             noenum::string::argument msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_severity const& loc_sev,
	                             string::argument_singulars<Enum> msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_severity const& loc_sev,
	                             string::argument_plurals<Enum> msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename SEnum, typename PEnum>
	inline diagnostic operator<<(
	    location_severity const& loc_sev,
	    string::argument_singulars_with_plurals<SEnum, PEnum> msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_severity const& loc_sev,
	                             string::formatable_singulars<Enum> msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_severity const& loc_sev,
	                             string::formatable_plurals<Enum> msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename SEnum, typename PEnum>
	inline diagnostic operator<<(
	    location_severity const& loc_sev,
	    string::formatable_singulars_with_plurals<SEnum, PEnum> msg) {
		return loc_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             noenum::string::message msg) {
		return {range_sev.start, range_sev.stop, range_sev.sev, std::move(msg)};
	}

	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             char const* msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::string_tag{}, msg);
	}

	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             std::string const& msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::string_tag{}, std::move(msg));
	}

	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             noenum::string::argument msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             string::argument_singulars<Enum> msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             string::argument_plurals<Enum> msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename SEnum, typename PEnum>
	inline diagnostic operator<<(
	    location_range_severity const& range_sev,
	    string::argument_singulars_with_plurals<SEnum, PEnum> msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename Enum>
	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             string::formatable_singulars<Enum> msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
		;
	}

	template <typename Enum>
	inline diagnostic operator<<(location_range_severity const& range_sev,
	                             string::formatable_plurals<Enum> msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}

	template <typename SEnum, typename PEnum>
	inline diagnostic operator<<(
	    location_range_severity const& range_sev,
	    string::formatable_singulars_with_plurals<SEnum, PEnum> msg) {
		return range_sev << noenum::string::message(
		           noenum::string::message::arg_tag{}, std::move(msg));
	}
}  // namespace diags
