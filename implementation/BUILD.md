# Building Cíonom Reference Implementation

You will need to run `git submodule update --init` after cloning if you didn't clone using `--recursive`.

To build from the Cíonom root directory run `make`. You may use `-j$(nproc)` in order to speed up the build.

This will output a `cionom` library into `lib/` and an executable at project root called `cionom-cli`.

Options for `cionom-cli` are detailed in [CLIREFERENCE.md](CLIREFERENCE.md).
