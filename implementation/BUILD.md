# Building Cíonom Reference Implementation

The Cíonom reference implementation uses Genstone and as such depends on everything outlined [here](https://github.com/Th3T3chn0G1t/Genstone/blob/trunk/README.md) in order to build, the same options also apply.

You will need to run `git submodule update --recursive --init` after cloning if you didn't clone using `git clone --recursive`.

To build from the Cíonom root directory run `make -Cimplementation/Genstone OVERRIDE_SANDBOX_PROJECT_MODULE=../cionom.mk OVERRIDE_DISABLED_MODULES="genfx vulkan spirv vulkan_validation"`. You may use `-j$(nproc)` in order to speed up the build.

This will output a `cionom` library into `implementation/Genstone/lib` and an executable at project root called `cionom-cli`. To run use `LD_LIBRARY_PATH=implementation/Genstone/lib ./cionom-cli.out` or `DYLD_LIBRARY_PATH=implementation/Genstone/lib ./cionom-cli.out` on macOS.
