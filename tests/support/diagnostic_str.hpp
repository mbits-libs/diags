// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/string.hpp>
#include <support/printer.hpp>
#include <variant>
#include <vector>

namespace diags::testing {
	struct diagnostic_str_base {
		std::variant<char const*,
		             noenum::string::singular,
		             noenum::string::plural>
		    heading;
		std::vector<diagnostic_str_base> args;

		diagnostic_str_base() : heading{""} {}

		template <typename... Args>
		diagnostic_str_base(char const* msg, Args&&... args)
		    : heading{msg}, args{std::forward<Args>(args)...} {}

		template <typename... Args>
		diagnostic_str_base(noenum::string::singular id, Args&&... args)
		    : heading{id}, args{std::forward<Args>(args)...} {}

		template <typename... Args>
		diagnostic_str_base(noenum::string::plural id, Args&&... args)
		    : heading{id}, args{std::forward<Args>(args)...} {}

		noenum::string::argument arg() const;

		friend std::ostream& operator<<(std::ostream& o,
		                                diagnostic_str_base const& rhs) {
			info(o, rhs.arg());
			return o;
		}
	};

	template <typename Enum>
	struct diagnostic_str_singular : diagnostic_str_base {
		diagnostic_str_singular() = default;

		template <typename... Args>
		diagnostic_str_singular(char const* msg, Args&&... args)
		    : diagnostic_str_base{
		          msg, diagnostic_str_singular{std::forward<Args>(args)}...} {}

		template <typename... Args>
		diagnostic_str_singular(Enum id, Args&&... args)
		    : diagnostic_str_base{
		          string::singular<Enum>{id},
		          diagnostic_str_singular{std::forward<Args>(args)}...} {}

		template <typename... Args>
		diagnostic_str_singular(string::singular<Enum> id, Args&&... args)
		    : diagnostic_str_base{
		          id, diagnostic_str_singular{std::forward<Args>(args)}...} {}
	};

	template <typename Enum>
	struct diagnostic_str_plural : diagnostic_str_base {
		diagnostic_str_plural() = default;

		template <typename... Args>
		diagnostic_str_plural(char const* msg, Args&&... args)
		    : diagnostic_str_base{msg, std::forward<Args>(args)...} {}

		template <typename... Args>
		diagnostic_str_plural(string::plural<Enum> id, Args&&... args)
		    : diagnostic_str_base{id, std::forward<Args>(args)...} {}
	};

	template <typename SEnum, typename PEnum>
	struct diagnostic_str_singular_with_plural : diagnostic_str_base {
		diagnostic_str_singular_with_plural() = default;

		template <typename... Args>
		diagnostic_str_singular_with_plural(char const* msg, Args&&... args)
		    : diagnostic_str_base{msg, std::forward<Args>(args)...} {}

		template <typename... Args>
		diagnostic_str_singular_with_plural(SEnum id, Args&&... args)
		    : diagnostic_str_base{string::singular<SEnum>{id},
		                          std::forward<Args>(args)...} {}

		template <typename... Args>
		diagnostic_str_singular_with_plural(string::singular<SEnum> id,
		                                    Args&&... args)
		    : diagnostic_str_base{id, std::forward<Args>(args)...} {}

		template <typename... Args>
		diagnostic_str_singular_with_plural(string::plural<PEnum> id,
		                                    Args&&... args)
		    : diagnostic_str_base{id, std::forward<Args>(args)...} {}
	};
}  // namespace diags::testing
