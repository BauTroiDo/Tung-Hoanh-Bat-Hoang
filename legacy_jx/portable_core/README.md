# Portable Core Bootstrap (CORE-37/38) — Version 0.1

STATUS: isolated implementation prototype. This is NOT a full JX Linux port or production Code Generator.

Provides an original C++17 library with:
- monotonic clock interface based on std::chrono::steady_clock;
- bounds-checked explicit little-endian binary primitives including u64;
- length-prefixed byte-string read/write with strict bounds;
- tests for golden endian bytes, malformed input, no partial read/write, 10000 deterministic fuzz samples;
- no Win32 or POSIX platform API includes in module source.

The code is self-contained and not linked into the existing JX server.
Out-of-tree CMake build required; do not place build output in legacy JX bin/lib/Release directories.

Note that byte strings are NOT validated UTF-8 here; generated semantic validators will do so.
Do not use extracted opcode enums to infer production packet lengths or layouts.
Linux native build not claimed unless compiled and tested with native Linux toolchain.
