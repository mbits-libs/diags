// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/string.hpp>
#include <lngs/lngs.hpp>

#define UNPACK_DIAGS_TYPES_LNGS() UNPACK_DIAGS_TYPES(Strings)
#define SINGLE_FORM_DIAGS_SUPPORT_LNGS() SINGLE_FORM_DIAGS_SUPPORT(lng)
#define PLURAL_FORM_DIAGS_SUPPORT_LNGS() PLURAL_FORM_DIAGS_SUPPORT(counted)

namespace diags::string {
	template <typename Enum, typename Storage>
	class string_traits<lngs::SingularStrings<Enum, Storage>>
	    : public string_traits<singular<Enum>> {};

	template <typename Enum, typename Storage>
	class string_traits<lngs::PluralOnlyStrings<Enum, Storage>>
	    : public string_traits<plural<Enum>> {};

	template <typename SEnum, typename PEnum, typename Storage>
	class string_traits<lngs::StringsWithPlurals<SEnum, PEnum, Storage>>
	    : public string_traits<both<SEnum, PEnum>> {};
}  // namespace diags::string
