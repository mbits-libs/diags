// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

namespace diags {
	enum class severity { verbose, note, warning, error, fatal };

	struct location_severity;

	struct location {
		unsigned token{};
		unsigned line{};
		unsigned column{};

		constexpr location moved_to(unsigned ln, unsigned col = 0) const {
			return {token, ln, col};
		}

		constexpr location moved_by(unsigned ln, unsigned col = 0) const {
			return {token, line + ln, column + col};
		}

		constexpr location moved_to_col(unsigned col) const {
			return {token, line, col};
		}

		constexpr location_severity operator[](severity sev) const;
	};

	struct location_severity {
		location loc;
		severity sev;
	};

	inline constexpr location_severity location::operator[](
	    severity sev) const {
		return {*this, sev};
	}

	struct location_range_severity {
		location start;
		location stop;
		severity sev;
	};

	struct location_range {
		location start;
		location stop;
		constexpr location_range_severity operator[](severity sev) const {
			return {start, stop, sev};
		}
	};

	inline constexpr location_range operator-(location const& start,
	                                          location const& stop) {
		return {start, stop};
	}

	inline constexpr location_range_severity operator-(
	    location const& start,
	    location_severity const& loc_sev) {
		return {start, loc_sev.loc, loc_sev.sev};
	}

}  // namespace diags