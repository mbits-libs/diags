// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <fmt/format.h>
#include <diags/string.hpp>

namespace diags::noenum::string {
	std::string singular::dbg() const {
		return "singular{" + std::to_string(id) + "}";
	}

	std::string plural::dbg() const {
		return "plural{" + std::to_string(id) + ", " + std::to_string(count) +
		       "}";
	}

	template <>
	std::string formatable_base<argument>::dbg() const {
		std::string result =
		    "formatable{subject:" +
		    std::visit(
		        [](auto const& arg) {
			        if constexpr (std::is_same_v<decltype(arg),
			                                     std::string const&>) {
				        return "\"" + arg + "\"s";
			        } else {
				        return arg.dbg();
			        }
		        },
		        subject_);
		if (!arguments_.empty()) {
			result += ", args:";
			char comma = '{';
			for (auto const& arg : arguments_) {
				result.push_back(comma);
				result.push_back(' ');
				comma = ',';
				result += arg.dbg();
			}
			result += " }";
		}
		return result + "}";
	}

	template <>
	std::string formatable_base<argument>::format(
	    diags::translator const& tr) const {
		auto subject = std::visit(
		    [&](auto const& arg) -> std::string_view {
			    if constexpr (std::is_same_v<decltype(arg), std::string const&>)
				    return arg;
			    else
				    return arg.translated(tr);
		    },
		    subject_);

		if (subject.empty()) return {};

		std::vector<std::string> storage;
		storage.reserve(arguments_.size());
		for (auto const& arg : arguments_)
			storage.push_back(arg.format(tr));

		auto args_data =
		    std::make_unique<fmt::format_args::format_arg[]>(arguments_.size());
		auto it = args_data.get();
		for (auto const& arg : storage)
			*it++ = fmt::detail::make_arg<fmt::format_context>(arg);
		fmt::format_args f_args{args_data.get(), arguments_.size()};

		return fmt::vformat(subject, f_args);
	}

	std::string argument::dbg() const {
		base_class const& slice_ref = *this;
		return std::visit(
		    [](auto const& arg) -> std::string {
			    if constexpr (std::is_same_v<decltype(arg),
			                                 std::string const&>) {
				    return "\"" + arg + "\"s";
			    } else {
				    return arg.dbg();
			    }
		    },
		    slice_ref);
	}

	std::string argument::format(diags::translator const& tr) const {
		base_class const& slice_ref = *this;
		return std::visit(
		    [&](auto const& arg) -> std::string {
			    if constexpr (std::is_same_v<decltype(arg),
			                                 std::string const&>) {
				    return arg;
			    } else if constexpr (std::is_same_v<decltype(arg),
			                                        singular const&> ||
			                         std::is_same_v<decltype(arg),
			                                        plural const&>) {
				    auto result = arg.translated(tr);
				    return {result.data(), result.size()};
			    } else if constexpr (std::is_same_v<decltype(arg),
			                                        formatable_type const&>) {
				    return arg.format(tr);
			    }
		    },
		    slice_ref);
	}
}  // namespace diags::noenum::string

namespace diags::string {}  // namespace diags::string
