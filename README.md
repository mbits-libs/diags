# diags

[![Travis (.org)][Travis badge]][Travis]
[![Coveralls github][Coveralls badge]][Coveralls]
[![Cpp Standard][17-badge]][17]

Open source library with file positions, diagnostic messages and source code hints.

## Example

```c++
using namespace diags;

sources host{};
auto text = host.open("README.md");

auto const start = text.position(1, 3);
auto const stop = text.position(1, 8);

auto const message =
	(start - stop)[severity::note]
		<< format("{}:", lng::THIS_IS_THE_NAME);

host.push_back(message);
```

With support for translations, this could be rendered as:

```
README.md:1:3: remarque: c'est le nom:
# diags
  ^~~~~
```

Or possibly as:

```
README.md(1,3): note: this is the name:
# diags
  ^~~~~
```

[Travis badge]: https://img.shields.io/travis/mbits-libs/diags?style=flat-square
[Travis]: https://travis-ci.org/mbits-libs/diags "Travis-CI"
[Coveralls badge]: https://img.shields.io/coveralls/github/mbits-libs/diags?style=flat-square
[Coveralls]: https://coveralls.io/github/mbits-libs/diags "Coveralls"
[17-badge]: https://img.shields.io/badge/C%2B%2B-17-informational?style=flat-square
[17]: https://en.wikipedia.org/wiki/C%2B%2B17 "Wikipedia C++17"
[20-badge]: https://img.shields.io/badge/C%2B%2B-20-informational?style=flat-square
