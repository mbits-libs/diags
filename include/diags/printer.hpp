// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <diags/diagnostic.hpp>
#include <diags/streams.hpp>

namespace diags {
	class sources;

	struct printer {
		printer(outstream* output,
		        translator const& tr,
		        link_type link = link_type::native)
		    : tr_{tr}, link_{link}, output_(output) {}
		virtual ~printer();

		void print(diagnostic const& input,
		           sources const& host);

		void print(std::vector<diagnostic> const& input,
		           sources const& host);

		static std::string filename_helper(location const&,
		                                   sources const&,
		                                   link_type);

		static std::pair<std::string, std::string> hint_lines_helper(
		    source_code& source,
		    location const& start,
		    location const& stop);

		struct line_indices {
			std::string line;
			size_t start_column;
			size_t stop_column;
		};

		static line_indices prepare_hint_lines(std::string_view,
		                                       size_t start_col,
		                                       size_t stop_col);

	protected:
		struct context {
			sources const& host;
			translator const tr;
			link_type const link;
			outstream* output;
			size_t depth{0};
		};

		virtual void print_one_diagnostic(diagnostic const&, context&) = 0;
		void print_recursive(diagnostic const&, context&);

	private:
		translator const tr_;
		link_type const link_;
		outstream* output_;
	};

	class basic_printer : public printer {
	public:
		using printer::printer;
		void print_one_diagnostic(diagnostic const&, context&) final;

	private:
		void print_message_line(diagnostic const&, context&);
	};
}  // namespace diags