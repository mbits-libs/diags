// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/diagnostic.hpp>
#include <diags/translator.hpp>
#include <support/diagnostic_str.hpp>
#include <support/strings.hpp>

namespace singular_strings {
	enum class lng {
		MESSAGE_A = 501,
		MESSAGE_B,
		MESSAGE_C,
		ARG_1,
		ARG_2,
	};  // enum class lng

	UNPACK_DIAGS_TYPES(diags::string::singular<lng>)
	SINGLE_FORM_DIAGS_SUPPORT(lng)
}  // namespace singular_strings

namespace diags::testing::singular {
	using namespace singular_strings;
	struct strings_mock : lng_strings<translator_type> {
		using lng_strings<translator_type>::get;
		std::string_view get(lng id) const noexcept final;
	};

	struct alt_strings_mock : alt_lng_strings<translator_type> {
		using alt_lng_strings<translator_type>::get;
		std::string_view get(lng id) const noexcept final;
	};

	using strings = strings<strings_mock, alt_strings_mock>;

	using diagnostic_str = diagnostic_str_singular<lng>;

	template <typename... Args>
	inline diagnostic_str str(char const* msg, Args&&... args) {
		return {msg, std::forward<Args>(args)...};
	}

	template <typename... Args>
	inline diagnostic_str str(lng id, Args&&... args) {
		return {id, std::forward<Args>(args)...};
	}

	template <typename... Args>
	inline diagnostic_str str(string::singular<lng> id, Args&&... args) {
		return {id, std::forward<Args>(args)...};
	}
}  // namespace diags::testing::singular
