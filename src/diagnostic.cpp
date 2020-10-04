// Copyright (c) 2018 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <diags/diagnostic.hpp>
#include <diags/sources.hpp>

namespace diags {
	size_t diagnostic::calc_lines(sources const& host) const noexcept {
		size_t counter{1};
		auto filename = host.filename(start_);
		if (!filename.empty() && start_.line && host.source(filename).valid())
			counter += 1 + (start_.column > 0);
		for (auto const& child : children_)
			counter += child.calc_lines(host);
		return counter;
	}

	void diagnostic::format(std::vector<std::string>& output,
	                        sources const& host,
	                        translator const& tr,
	                        link_type links,
	                        size_t depth) const {
		auto filename = host.filename(start_);

		output.push_back(message_line(filename, tr, links, depth));

		if (!filename.empty() && start_.line) {
			auto src = host.source(filename);
			if (src.valid()) {
				auto [text, indexes] = hint_lines(src);
				output.push_back(std::move(text));
				if (start_.column) output.push_back(std::move(indexes));
			}
		}

		for (auto const& child : children_)
			child.format(output, host, tr, links, depth + 1);
	}

	std::string diagnostic::message_line(std::string_view const& filename,
	                                     translator const& tr,
	                                     link_type links,
	                                     size_t depth) const {
		std::string msg_line{};
		if (links == link_type::vc) msg_line.append(depth * 4, ' ');
		if (severity_ != severity::verbose) {
			auto const link_col =
			    1 * (!start_.column) + 2 * (links == link_type::gcc);
			auto const index =
			    filename.empty() ? 5 : !start_.line ? 4 : link_col;

			static constexpr char const* links_fmt[] = {
			    "{0}({1},{2}): {3}: ",
			    "{0}({1}): {3}: ",
			    "{0}:{1}:{2}: {3}: ",
			    "{0}:{1}: {3}: ",
			    // 4 -- line zero
			    "{0}: {3}: ",
			    // 5 -- filename empty
			    "{3}: ",
			};

			msg_line += fmt::format(links_fmt[index], filename, start_.line,
			                        start_.column, tr.get(severity_));
		}

		msg_line += message_.format(tr);
		return msg_line;
	}

	std::pair<std::string, std::string> diagnostic::hint_lines(
	    source_code& source) const {
		std::pair<std::string, std::string> result{};

		auto [line, start_col, stop_col] =
		    prepare(source.line(start_.line), start_.column, stop_.column);

		result.first = std::move(line);

		if (start_.column) {
			result.second.reserve(stop_.column ? stop_col : start_col);
			result.second.assign(start_col - 1, ' ');
			result.second.push_back('^');
			if (stop_.column) {
				result.second.append(stop_col - start_col - 1, '~');
			}
		}

		return result;
	}

	diagnostic::line_indices diagnostic::prepare(
	    std::string_view line,
	    size_t start_col,
	    size_t stop_col) const noexcept {
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
}  // namespace diags
