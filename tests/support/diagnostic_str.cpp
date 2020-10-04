// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <support/diagnostic_str.hpp>

namespace diags::testing {
	noenum::string::argument diagnostic_str_base::arg() const {
		if (args.empty())
			return std::visit(
			    [](auto const& val) -> noenum::string::argument { return val; },
			    heading);

		std::vector<noenum::string::argument> offspring;
		offspring.reserve(args.size());
		for (auto const& a : args)
			offspring.push_back(a.arg());

		return std::visit(
		    [args = std::move(offspring)](auto const& val) mutable {
			    return noenum::string::formatable{val, std::move(args)};
		    },
		    heading);
	}
}  // namespace diags::testing
