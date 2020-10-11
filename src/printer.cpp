// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <fmt/format.h>
#include <diags/printer.hpp>
#include <diags/sources.hpp>
#include <numeric>

namespace diags {
	printer::printer(outstream* output,
	                 translator const& tr,
	                 color color_allowed,
	                 link_type link)
	    : tr_{tr}, link_{link}, output_(output), color_allowed_{color_allowed} {
		if (color_allowed_ == color::automatic) {
			color_allowed_ =
			    output && output->is_tty() ? color::always : color::never;
		}
	}
	printer::~printer() = default;

	void printer::print(diagnostic const& input, sources const& host) {
		context ctx{host, tr_, link_, output_, color_allowed_ == color::always};
		print_recursive(input, ctx);
	}

	void printer::print(std::vector<diagnostic> const& input,
	                    sources const& host) {
		context ctx{host, tr_, link_, output_, color_allowed_ == color::always};
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

	void printer::print_hint_lines(outstream* output,
	                               source_code& source,
	                               location const& start,
	                               location const& stop,
	                               std::string_view color_start,
	                               std::string_view color_stop) {
		auto [line, start_col, stop_col] = prepare_hint_lines(
		    source.line(start.line), start.column, stop.column);

		if (start_col) {
			--start_col;
			if (stop_col) --stop_col;
			if (stop_col <= start_col) stop_col = start_col + 1;
		} else {
			stop_col = 0;
		}

		if (start_col < (line.size() - 1)) {
			auto const local_stop =
			    stop_col > line.size() ? line.size() : stop_col;

			output->write(line.c_str(), start_col);
			output->write(color_start);
			output->write(line.c_str() + start_col, local_stop - start_col);
			output->write(color_stop);
			output->write(line.c_str() + stop_col, line.size() - local_stop);
		} else {
			output->write(line);
		}
		output->write('\n');

		if (start_col != stop_col) {
			for (size_t index = 0; index < start_col; ++index)
				output->write(' ');
			output->write(color_start);
			output->write('^');
			auto const wavy = stop_col - start_col - 1;
			for (size_t index = 0; index < wavy; ++index)
				output->write('~');
			output->write(color_stop);
			output->write('\n');
		}
	}

	std::pair<std::string_view, std::string_view> printer::severity_color(
	    severity sev,
	    bool color_allowed) noexcept {
		using color = std::pair<diags::severity,
		                        std::pair<std::string_view, std::string_view>>;
		static constexpr std::string_view reset = "\x1b[0m";
		static constexpr color colors[] = {
		    {diags::severity::verbose, {}},
		    {diags::severity::note, {"\x1b[1;36m", reset}},
		    {diags::severity::warning, {"\x1b[1;35m", reset}},
		    {diags::severity::error, {"\x1b[1;31m", reset}},
		};
		if (!color_allowed) return colors[0].second;
		for (auto [cmp, clr] : colors) {
			if (cmp == sev) return clr;
		}
		return colors[sizeof(colors) / sizeof(color) - 1].second;
	}

	void printer::print_recursive(diagnostic const& msg, context& ctx) {
		print_one_diagnostic(msg, ctx);
		++ctx.depth;
		for (auto const& child : msg.children()) {
			print_recursive(child, ctx);
		}
		--ctx.depth;
	}

	void printer::print_one_diagnostic(diagnostic const& msg, context& ctx) {
		print_message_line(msg, ctx);

		auto const start = msg.start();
		auto filename = ctx.host.filename(start);
		if (!filename.empty() && start.line) {
			auto src = ctx.host.source(filename);
			if (src.valid()) {
				auto const [start_color, reset_color] =
				    severity_color(msg.severity(), ctx.color_allowed);
				print_hint_lines(ctx.output, src, start, msg.stop(),
				                 start_color, reset_color);
			}
		}
	}

	void printer::print_message_line(diagnostic const& msg, context& ctx) {
		using namespace std::literals;
		auto const sev = msg.severity();
		if (sev != severity::verbose) {
			if (ctx.link == link_type::vc) {
				static constexpr std::string_view indent{"    "};
				for (size_t iter{}; iter < ctx.depth; ++iter)
					ctx.output->write(indent);
			}
			auto const path_line =
			    printer::filename_helper(msg.start(), ctx.host, ctx.link);
			if (!path_line.empty()) {
				if (ctx.color_allowed) ctx.output->write("\x1b[1m"sv);
				ctx.output->write(path_line);
				if (ctx.color_allowed)
					ctx.output->write(":\x1b[0m "sv);
				else
					ctx.output->write(": "sv);
			}

			auto const sev_str = ctx.tr.get(sev);
			if (!sev_str.empty()) {
				auto const [start_color, reset_color] =
				    severity_color(sev, ctx.color_allowed);
				ctx.output->write(start_color);
				ctx.output->write(sev_str);
				ctx.output->write(":"sv);
				ctx.output->write(reset_color);
				ctx.output->write(" "sv);
			}
		}

		auto const message = msg.message().format(ctx.tr);
		ctx.output->write(message);
		ctx.output->write('\n');
	}
}  // namespace diags
