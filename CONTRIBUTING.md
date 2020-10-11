# How to contribute

I'm really glad you're reading this. You are probably much further into this, than I am.

_This is very much work in progress._

In the meantime, look at the [issue templates](.github/ISSUE_TEMPLATE/).

Thanks, \
_Marcin_

### Things I still want to have:

- [x] Standalone printer
- [x] Color-based printer (color for severity and hints below the source line, bold for filename and position)
- [ ] `sources::open` / `source` / `set_contents` -- needs more `canonical` / `weakly_canonical`. 
- [ ] Print-on-push as alternative to printing through `sources::diagnostic_set()`.
- [ ] Bold/color inside `string::message` (important pieces of information)
- [ ] Warning-based message filters.
- [ ] _Low priority:_ way towards switching from {fmt} to C++20 / `std::format` as an alternative to `fmt::format`.
- [ ] Possibly, formatter for `string::argument`.

### Test for `canonical`:

#### Setup:

- `data/dirname`, `data/the_raven.txt`, `data/not_the_raven.txt`
- `pathA` is `TESTING_data_path/"the_raven.txt"`
- `pathB` is `"the_raven.txt"`
- `pathC` is `"dirname/../the_raven.txt"`
- `pathD` is `"dirname/../not_the_raven.txt"`
- CWD-1 is `TESTING_data_path`
- CWD-2 is `$TMP`

#### Test cases:

1. With CWD-1 active, \
   `sources.source(pathA).position()` ==
   `sources.source(pathB).position()`.
2. With CWD-1 active, \
   `sources.source(pathA).position()` ==
   `sources.source(pathC).position()`.
3. With CWD-1 active, \
   `sources.source(pathB).position()` !=
   `sources.source(pathD).position()`.
4. With CWD-2 active, \
   `sources.source(pathA).position()` !=
    `sources.source(pathB).position()`.
5. With CWD-2 active, \
   `sources.source(pathA).position()` !=
    `sources.source(pathC).position()`.
6. With CWD-1 active, `pathA` used before `pathB` and `pathC`, \
   `sources.filename(sources.source(pathA).position())` == `pathA` and \
   `sources.filename(sources.source(pathB).position())` == `pathA` and \
   `sources.filename(sources.source(pathC).position())` == `pathA`
7. With CWD-1 active, `pathB` used before `pathA` and `pathC`, \
   `sources.filename(sources.source(pathA).position())` == `pathB` and \
   `sources.filename(sources.source(pathB).position())` == `pathB` and \
   `sources.filename(sources.source(pathC).position())` == `pathB`
8. With CWD-1 active, `pathC` used before `pathA` and `pathB`, \
   `sources.filename(sources.source(pathA).position())` == `pathC` and \
   `sources.filename(sources.source(pathB).position())` == `pathC` and \
   `sources.filename(sources.source(pathC).position())` == `pathC`
