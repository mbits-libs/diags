// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/location.hpp>
#include <diags/translator.hpp>
#include <string_view>

namespace diags::testing {
	template <typename Strings, typename AltStrings>
	struct strings {
		Strings strings{};
		AltStrings alt_strings{};

		template <typename TranslatorType>
		translator choose(bool use_alt) {
			return choose<TranslatorType>(use_alt, strings, alt_strings);
		}

	private:
		template <typename TranslatorType>
		static translator choose(bool use_alt,
		                         TranslatorType& strings,
		                         TranslatorType& alt_strings) {
			return (use_alt ? alt_strings : strings).make();
		}
	};

	struct sev_strings {
		std::string_view get_std(severity sev) const noexcept;
		std::string_view get_alt(severity sev) const noexcept;
	};

	template <typename TranslatorType>
	struct lng_strings : TranslatorType, sev_strings {
		using TranslatorType::get;
		std::string_view get(severity sev) const noexcept final {
			return this->get_std(sev);
		}
	};

	template <typename TranslatorType>
	struct alt_lng_strings : TranslatorType, sev_strings {
		using TranslatorType::get;
		std::string_view get(severity sev) const noexcept final {
			return this->get_alt(sev);
		}
	};
}  // namespace diags::testing
