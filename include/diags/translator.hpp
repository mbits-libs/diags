// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <cstdint>
#include <diags/location.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

namespace diags {
	struct translator_base {
		virtual ~translator_base();
		virtual std::string_view get(severity) const noexcept = 0;
	};

	struct translator {
		translator_base* tr;
		std::string_view (*singulars_cb)(translator_base const*, unsigned);
		std::string_view (*plurals_cb)(translator_base const*,
		                               unsigned,
		                               intmax_t);

		std::string_view get(severity sev) const { return tr->get(sev); }

		std::string_view get(unsigned id) const { return singulars_cb(tr, id); }

		std::string_view get(unsigned id, intmax_t count) const {
			return plurals_cb(tr, id, count);
		}
	};
}  // namespace diags

namespace diags::tr {
	template <typename Translator, typename Enum>
	struct impl_singulars {
		static std::string_view call(translator_base const* translator,
		                             unsigned id) {
			return static_cast<Translator const*>(translator)
			    ->get(static_cast<Enum>(id));
		}
	};

	template <typename Translator, typename Enum>
	struct impl_plurals {
		static std::string_view call(translator_base const* translator,
		                             unsigned id,
		                             intmax_t count) {
			return static_cast<Translator const*>(translator)
			    ->get(static_cast<Enum>(id), count);
		}
	};

	template <typename... Args>
	struct impl_purecall {
		static std::string_view call(translator_base const*, Args...) {
			throw std::runtime_error("Calling wrong method");
		}
	};
}  // namespace diags::tr

namespace diags {
	template <typename Enum, typename Interface = translator_base>
	struct translator_singulars : Interface {
		using Interface::get;
		virtual std::string_view get(Enum) const noexcept = 0;

		translator make() {
			using this_class = translator_singulars<Enum, Interface>;
			return {this, tr::impl_singulars<this_class, Enum>::call,
			        tr::impl_purecall<unsigned, intmax_t>::call};
		}
	};

	template <typename Enum, typename Interface = translator_base>
	struct translator_plurals : Interface {
		using Interface::get;
		virtual std::string_view get(Enum, intmax_t) const noexcept = 0;

		translator make() {
			using this_class = translator_plurals<Enum, Interface>;
			return {this, tr::impl_purecall<unsigned>::call,
			        tr::impl_plurals<this_class, Enum>::call};
		}
	};

	template <typename SEnum,
	          typename PEnum,
	          typename Interface = translator_base>
	struct translator_singulars_and_plurals
	    : translator_singulars<SEnum, translator_plurals<PEnum, Interface>> {
		using translator_singulars<SEnum,
		                           translator_plurals<PEnum, Interface>>::get;

		translator make() {
			using this_class =
			    translator_singulars_and_plurals<SEnum, PEnum, Interface>;
			return {this, tr::impl_singulars<this_class, SEnum>::call,
			        tr::impl_plurals<this_class, PEnum>::call};
		}
	};
}  // namespace diags
