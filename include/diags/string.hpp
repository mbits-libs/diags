// Copyright (c) 2020 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <diags/translator.hpp>
#include <variant>
#include <vector>

// define DIAGS_EMPTY_FORMATABLE_COMPARES_TO_SUBJECT

#define UNPACK_DIAGS_TYPES(...)                                        \
	using string_traits = diags::string::string_traits<__VA_ARGS__>;   \
	using formatable = string_traits::formatable_type;                 \
	using argument = string_traits::argument_type;                     \
	using translator_type = string_traits::translator_type;            \
                                                                       \
	template <typename FirstArg, typename... Args>                     \
	[[nodiscard]] formatable format(std::string fmt, FirstArg first,   \
	                                Args... args) {                    \
		return string_traits::format(std::move(fmt), std::move(first), \
		                             std::move(args)...);              \
	}

#define SINGLE_FORM_DIAGS_SUPPORT(ENUM_TYPE)                             \
	template <typename FirstArg, typename... Args>                       \
	[[nodiscard]] formatable format(ENUM_TYPE id, FirstArg first,        \
	                                Args... args) {                      \
		return string_traits::format(id, std::move(first),               \
		                             std::move(args)...);                \
	}                                                                    \
                                                                         \
	[[nodiscard]] inline diags::diagnostic operator<<(                   \
	    diags::location_severity const& loc_sev, ENUM_TYPE id) {         \
		return loc_sev << argument{id};                                  \
	}                                                                    \
                                                                         \
	[[nodiscard]] inline diags::diagnostic operator<<(                   \
	    diags::location_range_severity const& range_sev, ENUM_TYPE id) { \
		return range_sev << argument{id};                                \
	}

#define PLURAL_FORM_DIAGS_SUPPORT(ENUM_TYPE)                             \
	template <typename FirstArg, typename... Args>                       \
	[[nodiscard]] formatable format(diags::string::plural<ENUM_TYPE> id, \
	                                FirstArg first, Args... args) {      \
		return string_traits::format(id, std::move(first),               \
		                             std::move(args)...);                \
	}                                                                    \
                                                                         \
	[[nodiscard]] inline diags::string::plural<ENUM_TYPE> operator/(     \
	    ENUM_TYPE id, intmax_t count) {                                  \
		return {id, count};                                              \
	}

namespace diags::noenum::string {
	template <typename Enum>
	inline static unsigned clear_enum(Enum e) {
		return static_cast<unsigned>(
		    static_cast<std::underlying_type_t<Enum>>(e));
	}
	struct singular {
		unsigned id{};

		singular() = default;

		singular(unsigned id) : id{id} {}

		std::string dbg() const;

		bool operator==(singular const& right) const noexcept {
			return id == right.id;
		}

		bool operator!=(singular const& right) const noexcept {
			return !(*this == right);
		}

		std::string_view translated(diags::translator const& tr) const {
			return tr.get(id);
		}
	};

	struct plural {
		unsigned id{};
		intmax_t count{};

		plural() = default;

		plural(unsigned id, intmax_t count) : id{id}, count{count} {}

		std::string dbg() const;

		bool operator==(plural const& right) const noexcept {
			return id == right.id && count == right.count;
		}

		bool operator!=(plural const& right) const noexcept {
			return !(*this == right);
		}

		std::string_view translated(diags::translator const& tr) const {
			return tr.get(id, count);
		}
	};

	template <typename ArgType>
	struct formatable_base {
	public:
		using subject_type = std::variant<std::string, singular, plural>;

		formatable_base() = default;

		formatable_base(std::string str, std::vector<ArgType> arguments)
		    : subject_{std::move(str)}, arguments_{std::move(arguments)} {}

		formatable_base(char const* str, std::vector<ArgType> arguments)
		    : subject_{str}, arguments_{std::move(arguments)} {}

		formatable_base(singular id, std::vector<ArgType> arguments)
		    : subject_{std::move(id)}, arguments_{std::move(arguments)} {}

		formatable_base(plural id, std::vector<ArgType> arguments)
		    : subject_{std::move(id)}, arguments_{std::move(arguments)} {}

		std::string dbg() const;

		bool operator==(formatable_base const& right) const noexcept {
			return subject_ == right.subject_ &&
			       arguments_.size() == right.arguments_.size() &&
			       args_items_equal(right.arguments_);
		}

		bool operator!=(formatable_base const& right) const noexcept {
			return !(*this == right);
		}

		inline std::string format(diags::translator const& tr) const;

		subject_type const& subject() const noexcept { return subject_; }

		subject_type& subject() noexcept { return subject_; }

		std::vector<ArgType> const& arguments() const noexcept {
			return arguments_;
		}

		std::vector<ArgType>& arguments() noexcept { return arguments_; }

	private:
		inline bool args_items_equal(
		    std::vector<ArgType> const& right_args) const noexcept {
			auto it = arguments_.begin();
			auto end = arguments_.end();
			auto r_it = right_args.begin();
			for (; it != end; ++it, ++r_it) {
				auto const& l_arg = *it;
				auto const& r_arg = *r_it;
				if (l_arg != r_arg) return false;
			}
			return true;
		}

		subject_type subject_{};
		std::vector<ArgType> arguments_{};
	};

	struct argument : std::variant<std::string,
	                               singular,
	                               plural,
	                               formatable_base<argument>> {
		using formatable_type = formatable_base<argument>;
		using base_class =
		    std::variant<std::string, singular, plural, formatable_type>;

		argument() = default;
		argument(argument const&) = default;
		argument& operator=(argument const&) = default;
		argument(argument&&) = default;
		argument& operator=(argument&&) = default;

		argument(char const* str) : base_class{str} {}

		argument(std::string str)
		    : base_class{std::in_place_type<std::string>, std::move(str)} {}

		argument(singular str)
		    : base_class{std::in_place_type<singular>, str} {}

		argument(plural str) : base_class{std::in_place_type<plural>, str} {}

		argument(unsigned id) : base_class{std::in_place_type<singular>, id} {}

		argument(unsigned id, intmax_t count)
		    : base_class{std::in_place_type<plural>, id, count} {}

		argument(formatable_type str)
		    : base_class{std::in_place_type<formatable_type>, std::move(str)} {}

		std::string dbg() const;

		bool operator==(argument const& right_arg) const noexcept {
			auto& left = static_cast<base_class const&>(*this);
			auto& right = static_cast<base_class const&>(right_arg);
#ifdef DIAGS_EMPTY_FORMATABLE_COMPARES_TO_SUBJECT
			if (left.index() != right.index()) {
				if (holds_empty_formatable()) return right_arg == subject();
				if (right_arg.holds_empty_formatable())
					return *this == right_arg.subject();
			}
#endif
			return left == right;
		}

#ifdef DIAGS_EMPTY_FORMATABLE_COMPARES_TO_SUBJECT
		bool operator==(formatable_type::subject_type const& subject) const {
			base_class const& ref = *this;
			return std::visit(
			    [](auto const& left, auto const& right) {
				    if constexpr (!std::is_same_v<decltype(left),
				                                  decltype(right)>) {
					    return false;
				    } else {
					    return left == right;
				    }
			    },
			    ref, subject);
		}
#endif

		bool operator!=(argument const& right) const noexcept {
			return !(*this == right);
		}

		std::string format(diags::translator const& tr) const;

	private:
		inline bool holds_empty_formatable() const noexcept {
			base_class const& ref = *this;
			return std::holds_alternative<formatable_type>(ref) &&
			       std::get<formatable_type>(ref).arguments().empty();
		}

#ifdef DIAGS_EMPTY_FORMATABLE_COMPARES_TO_SUBJECT
		inline formatable_type::subject_type const& subject() const noexcept {
			base_class const& ref = *this;
			return std::get<formatable_type>(ref).subject();
		}
#endif
	};

	using formatable = typename argument::formatable_type;

	struct message : argument {
		message() = default;
		message(message const&) = default;
		message& operator=(message const&) = default;
		message(message&&) = default;
		message& operator=(message&&) = default;

		struct arg_tag {};
		message(arg_tag, argument const& other) : argument{other} {}
		message(arg_tag, argument&& other) : argument{std::move(other)} {}

		struct string_tag {};
		message(string_tag, char const* str) : argument{str} {}
		message(string_tag, std::string str) : argument{std::move(str)} {}

		bool operator==(message const& right_arg) const noexcept {
			auto& left = static_cast<base_class const&>(*this);
			auto& right = static_cast<base_class const&>(right_arg);
			return left == right;
		}

		bool operator!=(message const& right) const noexcept {
			return !(*this == right);
		}
	};
}  // namespace diags::noenum::string

namespace diags::string {
	template <typename Enum>
	struct singular : noenum::string::singular {
		singular() = default;

		singular(Enum id)
		    : noenum::string::singular{noenum::string::clear_enum(id)} {}
	};

	template <typename Enum>
	struct formatable_singulars : noenum::string::formatable {
		using base_class = noenum::string::formatable;
		using Arguments = std::vector<noenum::string::argument>;

		formatable_singulars() = default;

		formatable_singulars(char const* str, Arguments arguments)
		    : base_class{str, std::move(arguments)} {}
		formatable_singulars(std::string str, Arguments arguments)
		    : base_class{std::move(str), std::move(arguments)} {}
		formatable_singulars(Enum id, Arguments arguments)
		    : base_class{singular<Enum>(id), std::move(arguments)} {}
	};

	template <typename Enum>
	struct argument_singulars : noenum::string::argument {
		using base_class = noenum::string::argument;

		argument_singulars() = default;
		argument_singulars(char const* str) : base_class{str} {}
		argument_singulars(std::string str) : base_class{std::move(str)} {}
		argument_singulars(Enum id) : base_class{singular<Enum>(id)} {}
		argument_singulars(singular<Enum> id) : base_class{id} {}
		argument_singulars(formatable_singulars<Enum> str)
		    : base_class{std::move(str)} {}
	};

	template <typename Enum>
	struct plural : noenum::string::plural {
		plural() = default;

		plural(Enum id, intmax_t count)
		    : noenum::string::plural{noenum::string::clear_enum(id), count} {}
	};

	template <typename Enum>
	struct formatable_plurals : noenum::string::formatable {
		using base_class = noenum::string::formatable;
		using Arguments = std::vector<noenum::string::argument>;

		formatable_plurals() = default;

		formatable_plurals(char const* str, Arguments arguments)
		    : base_class{str, std::move(arguments)} {}
		formatable_plurals(std::string str, Arguments arguments)
		    : base_class{std::move(str), std::move(arguments)} {}
		formatable_plurals(plural<Enum> id, Arguments arguments)
		    : base_class{id, std::move(arguments)} {}
	};

	template <typename Enum>
	struct argument_plurals : noenum::string::argument {
		using base_class = noenum::string::argument;

		argument_plurals() = default;
		argument_plurals(char const* str) : base_class{str} {}
		argument_plurals(std::string str) : base_class{std::move(str)} {}
		argument_plurals(plural<Enum> id) : base_class{id} {}
		argument_plurals(formatable_plurals<Enum> str)
		    : base_class{std::move(str)} {}
	};

	template <typename SEnum, typename PEnum>
	struct both {};

	template <typename SEnum, typename PEnum>
	struct formatable_singulars_with_plurals : noenum::string::formatable {
		using base_class = noenum::string::formatable;
		using Arguments = std::vector<noenum::string::argument>;

		formatable_singulars_with_plurals() = default;

		formatable_singulars_with_plurals(char const* str, Arguments arguments)
		    : base_class{str, std::move(arguments)} {}
		formatable_singulars_with_plurals(std::string str, Arguments arguments)
		    : base_class{std::move(str), std::move(arguments)} {}
		formatable_singulars_with_plurals(SEnum id, Arguments arguments)
		    : base_class{singular<SEnum>{id}, std::move(arguments)} {}
		formatable_singulars_with_plurals(singular<SEnum> id,
		                                  Arguments arguments)
		    : base_class{id, std::move(arguments)} {}
		formatable_singulars_with_plurals(plural<PEnum> id, Arguments arguments)
		    : base_class{id, std::move(arguments)} {}
	};

	template <typename SEnum, typename PEnum>
	struct argument_singulars_with_plurals : noenum::string::argument {
		using base_class = noenum::string::argument;

		argument_singulars_with_plurals() = default;
		argument_singulars_with_plurals(char const* str) : base_class{str} {}
		argument_singulars_with_plurals(std::string str)
		    : base_class{std::move(str)} {}
		argument_singulars_with_plurals(SEnum id)
		    : base_class{singular<SEnum>{id}} {}
		argument_singulars_with_plurals(singular<SEnum> id) : base_class{id} {}
		argument_singulars_with_plurals(plural<PEnum> id) : base_class{id} {}
		argument_singulars_with_plurals(
		    formatable_singulars_with_plurals<SEnum, PEnum> str)
		    : base_class{std::move(str)} {}
	};

	template <typename StringKind>
	struct string_traits;

	template <typename Enum>
	struct string_traits<singular<Enum>> {
		using translator_type = translator_singulars<Enum>;
		using formatable_type = formatable_singulars<Enum>;
		using argument_type = argument_singulars<Enum>;

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(std::string format,
		                                            FirstArg first,
		                                            Args... args) {
			return {std::move(format),
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(Enum id,
		                                            FirstArg first,
		                                            Args... args) {
			return {id,
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}
	};

	template <typename Enum>
	struct string_traits<plural<Enum>> {
		using translator_type = translator_plurals<Enum>;
		using formatable_type = formatable_plurals<Enum>;
		using argument_type = argument_plurals<Enum>;

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(std::string frmt,
		                                            FirstArg first,
		                                            Args... args) {
			return {std::move(frmt),
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(plural<Enum> id,
		                                            FirstArg first,
		                                            Args... args) {
			return {id,
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}
	};

	template <typename SEnum, typename PEnum>
	struct string_traits<both<SEnum, PEnum>> {
		using translator_type = translator_singulars_and_plurals<SEnum, PEnum>;
		using formatable_type = formatable_singulars_with_plurals<SEnum, PEnum>;
		using argument_type = argument_singulars_with_plurals<SEnum, PEnum>;

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(std::string format,
		                                            FirstArg first,
		                                            Args... args) {
			return {std::move(format),
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(SEnum id,
		                                            FirstArg first,
		                                            Args... args) {
			return {id,
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}

		template <typename FirstArg, typename... Args>
		[[nodiscard]] static formatable_type format(plural<PEnum> id,
		                                            FirstArg first,
		                                            Args... args) {
			return {id,
			        {argument_type(std::move(first)),
			         argument_type(std::move(args))...}};
		}
	};
}  // namespace diags::string
