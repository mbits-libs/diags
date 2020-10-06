// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <diags/location.hpp>
#include <diags/streams.hpp>
#include <diags/string.hpp>
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
