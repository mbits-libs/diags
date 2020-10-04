// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <support/strings.hpp>

using namespace std::literals;

namespace diags::testing {
	std::string_view sev_strings::get_std(severity sev) const noexcept {
		switch (sev) {
			case severity::note:
				return "note"sv;
			case severity::warning:
				return "warning"sv;
			case severity::error:
				return "error"sv;
			case severity::fatal:
				return "fatal"sv;
			default:
				break;
		}
		return {};
	}

	std::string_view sev_strings::get_alt(severity sev) const noexcept {
		switch (sev) {
			case severity::note:
				return "nnn"sv;
			case severity::warning:
				return "www"sv;
			case severity::error:
				return "eee"sv;
			case severity::fatal:
				return "fff"sv;
			default:
				break;
		}
		return {};
	}
}  // namespace diags::testing
