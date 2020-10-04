# Usage of {fmt} vs C++20

1. `streams.hpp` -- `string_view`, `format_arg_store` / `format_args`, `format_context`
2. `streams.cpp` -- `string_view`, `format_args`, `memory_buffer`, `vformat_to()`
3. `diagnostic.cpp` -- `format()`
4. `string.cpp` -- `format_args::format_arg`, `format_args`, `format_context`, `detail::make_arg`, `vformat()`

|{fmt}|C++20|
|-----|-----|
| `fmt::format()` | [`std::format()`](https://en.cppreference.com/w/cpp/utility/format/format) |
| `fmt::vformat()` | [`std::vformat()`](https://en.cppreference.com/w/cpp/utility/format/vformat) |
| `fmt::vformat_to()` | [`std::vformat_to()`](https://en.cppreference.com/w/cpp/utility/format/vformat_to) |
| `detail::make_arg()` | [`std::make_format_args()`](https://en.cppreference.com/w/cpp/utility/format/make_format_args)? |
| `format_arg_store` | Unspecified, but it exists |
| `format_args::format_arg` | [`std::basic_format_arg`](https://en.cppreference.com/w/cpp/utility/format/basic_format_arg) |
| `format_args` | [`std::basic_format_args`](https://en.cppreference.com/w/cpp/utility/format/basic_format_args) |
| `format_context` | [`std::basic_format_context`](https://en.cppreference.com/w/cpp/utility/format/basic_format_context) |
| `memory_buffer` | See below |
| `string_view` | `std::string_view` |

```c++
std::vector<char> buf;
std::format_to(std::back_inserter(buf), "{}", 42);
// buf contains "42"
 ```
