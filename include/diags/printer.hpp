// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <diags/diagnostic.hpp>
#include <diags/streams.hpp>

namespace diags {
	class sources;

	enum class color { never, always, automatic };

	struct printer {
		printer(outstream* output,
		        translator const& tr,
		        color color_allowed,
		        link_type link = link_type::native);
		virtual ~printer();

		void print(diagnostic const& input, sources const& host);

		void print(std::vector<diagnostic> const& input, sources const& host);

		static std::string filename_helper(location const&,
		                                   sources const&,
		                                   link_type);

		struct line_indices {
			std::string line;
			size_t start_column;
			size_t stop_column;
		};

		static line_indices prepare_hint_lines(std::string_view,
		                                       size_t start_col,
		                                       size_t stop_col);

		static void print_hint_lines(outstream* output,
		                             source_code& source,
		                             location const& start,
		                             location const& stop,
		                             std::string_view color_start = {},
		                             std::string_view color_stop = {});

		static std::pair<std::string_view, std::string_view> severity_color(
		    severity,
		    bool color_allowed) noexcept;

	protected:
		struct context {
			sources const& host;
			translator const tr;
			link_type const link;
			outstream* output;
			bool color_allowed;
			size_t depth{0};
		};

		virtual void print_one_diagnostic(diagnostic const&, context&);
		virtual void print_message_line(diagnostic const&, context&);
		void print_recursive(diagnostic const&, context&);

		translator const tr_;
		link_type const link_;
		outstream* output_;
		color color_allowed_;
	};
}  // namespace diags