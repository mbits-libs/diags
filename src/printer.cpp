// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <fmt/format.h>
#include <diags/printer.hpp>
#include <diags/sources.hpp>
#include <numeric>

namespace diags {
	printer::~printer() = default;

	void printer::print(diagnostic const& input, sources const& host) {
		context ctx{host, tr_, link_, output_};
		print_recursive(input, ctx);
	}

	void printer::print(std::vector<diagnostic> const& input,
	                    sources const& host) {
   		context ctx{host, tr_, link_, output_};
		for (auto const& msg : input) {
			print_recursive(msg, ctx);
		}
	}

	std::string printer::filename_helper(location const& pos,
	                                     sources const& host,
	                                     link_type link) {
		auto filename = host.filename(pos);
		if (filename.empty()) return {};

		auto const link_col = 1 * (!pos.column) + 2 * (link == link_type::gcc);
		auto const index = pos.line ? link_col : 4;

		static constexpr char const* links_fmt[] = {
		    "{0}({1},{2})",
		    "{0}({1})",
		    "{0}:{1}:{2}",
		    "{0}:{1}",
		    // 4 -- line zero
		    "{0}",
		};

		auto u8filename = filename.u8string();
		auto filepath =
		    std::string_view{reinterpret_cast<char const*>(u8filename.data()),
		                     u8filename.size()};

		return fmt::format(links_fmt[index], filepath, pos.line, pos.column);
	}

	std::pair<std::string, std::string> printer::hint_lines_helper(
	    source_code& source,
	    location const& start,
	    location const& stop) {
		std::pair<std::string, std::string> result{};

		auto [line, start_col, stop_col] = prepare_hint_lines(
		    source.line(start.line), start.column, stop.column);

		result.first = std::move(line);

		if (start.column) {
			result.second.reserve(stop.column ? stop_col : start_col);
			result.second.assign(start_col - 1, ' ');
			result.second.push_back('^');
			if (stop.column) {
				result.second.append(stop_col - start_col - 1, '~');
			}
		}

		return result;
	}

	printer::line_indices printer::prepare_hint_lines(std::string_view line,
	                                                  size_t start_col,
	                                                  size_t stop_col) {
		struct index {
			size_t raw, mapped{0};
		} col_start{start_col}, col_stop{stop_col};

		size_t len = 0;
		size_t pos = 0;

		for (auto c : line) {
			++len;
			++pos;
			if (col_start.raw == pos) col_start.mapped = len;
			if (col_stop.raw == pos) col_stop.mapped = len;
			if (c == '\t') {
				while (len % diagnostic::tab_size) {
					++len;
				}
			}
		}

		if (col_start.raw && !col_start.mapped)
			col_start.mapped = len + (col_start.raw - pos);
		if (col_stop.raw && !col_stop.mapped)
			col_stop.mapped = len + (col_stop.raw - pos);

		line_indices result{{}, col_start.mapped, col_stop.mapped};
		result.line.reserve(len);

		len = 0;
		for (auto c : line) {
			++len;
			if (c == '\t') {
				result.line.push_back(' ');
				while (len % diagnostic::tab_size) {
					++len;
					result.line.push_back(' ');
				}
			} else
				result.line.push_back(c);
		}

		return result;
	}

	void printer::print_recursive(diagnostic const& msg, context& ctx) {
		print_one_diagnostic(msg, ctx);
		++ctx.depth;
		for (auto const& child : msg.children()) {
			print_recursive(child, ctx);
		}
		--ctx.depth;
	}

	void basic_printer::print_one_diagnostic(diagnostic const& msg,
	                                         context& ctx) {
		print_message_line(msg, ctx);

		auto const start = msg.start();
		auto filename = ctx.host.filename(start);
		if (!filename.empty() && start.line) {
			auto src = ctx.host.source(filename);
			if (src.valid()) {
				auto [text, indexes] =
					hint_lines_helper(src, start, msg.stop());
				ctx.output->write(text);
				ctx.output->write('\n');
				if (start.column) {
					ctx.output->write(indexes);
					ctx.output->write('\n');
				}
			}
		}
	}

	void basic_printer::print_message_line(diagnostic const& msg,
	                                       context& ctx) {
		using namespace std::literals;
		auto const sev = msg.severity();
		if (sev != severity::verbose) {
            if (ctx.link == link_type::vc) {
                static constexpr std::string_view indent{"    "};
				for (size_t iter{}; iter < ctx.depth; ++iter)
                    ctx.output->write(indent);
            }
			auto const path_line = printer::filename_helper(msg.start(), ctx.host, ctx.link);
			if (!path_line.empty()) {
				ctx.output->write(path_line);
				ctx.output->write(": "sv);
			}

			auto const sev_str = ctx.tr.get(sev);
			if (!sev_str.empty()) {
				ctx.output->write(sev_str);
				ctx.output->write(": "sv);
			}
		}
		
		auto const message = msg.message().format(ctx.tr);
		ctx.output->write(message);
        ctx.output->write('\n');
	}
}  // namespace diags
