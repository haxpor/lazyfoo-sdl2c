# NOTE

This sample also includes `vector` implementation for specific struct and general purpose one.

Please see relevant files `vectorDot.h`, `vectorDot.c`, `vector.h`, `vector.c` along with testing file `vectorDot_test.c` and `vector_test.c` too see how to use it.

This vector is **not** used in main tuturial source code but included as another alternative using instead of pointer as used in the tutorial source file.
As well for standalone vector that is detached from SDL2, take a look at [haxpor/vector_c](https://github.com/haxpor/vector_c).

# Build vector

To test how vector works,

use `make vectorDot` and `make vector` to build `vectorDot` and `vector` executable files.

# Build sample code

Use `make` to build but it will include `assert` in the build.

To not include it, use `make CFLAGS=-DNDEBUG`. You can check result executable file with `objdump -t <file.out> | grep assert` to see whether `assert` symbol is included in the binary file or not.
