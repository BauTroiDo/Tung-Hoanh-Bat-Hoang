# Linux x86_64 legacy Engine ABI milestone

**Scope: Linux ABI fixes and native tests for two legacy JX Engine source files. This is NOT a Linux GameServer release.**

The original Windows-only `KWin32.h` has a partial Linux branch. On Linux
x86_64, `long` and `unsigned long` are 64-bit but on Windows x64
`LONG` and `DWORD` are 32-bit. That widened original `RECT`, `POINT`
and `GUID` layouts. This change fixes the Linux typedefs, exports
`ENGINE_API` through a GCC/Clang-compatible macro, and makes the existing
MSVC pragma/string macro acceptable to Linux compilers. The Windows MSVC
definitions are otherwise unchanged, and original source encoding retained.

Tests include the real `KWin32.h` and compile the real legacy
`KNode.cpp` and `KList.cpp` into a Linux static library. `linux_abi_test`
verifies fixed-width types and struct offsets. `engine_node_list_smoke`
tests real node insertion/removal. These are C++/Linux tests only; they do
not open a network listener, use customer data, or start a GameServer.

```bash
cmake -S legacy_jx/tests/linux_abi -B /tmp/jx-linux-abi -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/jx-linux-abi
ctest --test-dir /tmp/jx-linux-abi --output-on-failure
```

For sanitizer verification, configure a separate build directory using
`-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined` and
`-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined`, then build and
run CTest again.

**Verified:** Ubuntu 26.04 WSL, GCC 15.2, CMake 4.2, CTest 2/2 PASS
normally and 2/2 PASS under ASan/UBSan.

**Not yet supported:** full Linux Engine build, Lua/WorldServer/GameServer
Linux compilation, `gs001` ELF binary, runtime login, NPC/map gameplay,
or authentic JX V9 wire verification. The original `KNode.h` still produces
legacy pragma/no-op assert warnings. Linux port should proceed module by
module, avoiding unsupported WinAPI stub headers.

## 2026-10-21 follow-up: native KTimer (3/3 tests)

The original `KTimer.cpp` failed Linux compilation because it included
`Winbase.h` unconditionally, used a Windows-only `gettimeofday` shim,
referenced non-existent Linux `m_nFrequency`, and returned zero for
`GetInterval()`. Wall-clock `gettimeofday` can jump if the clock is
adjusted, corrupting gameplay duration calculations.

The Linux-only implementation now uses `clock_gettime(CLOCK_MONOTONIC)`
and `timespec`, preserving existing Windows `QueryPerformanceCounter`
behavior within `WIN32` branches. `GetElapse()` and `GetInterval()`
return milliseconds; Linux `GetElapseFrequency()` returns elapsed
high-resolution **nanoseconds**, not a frequency value in hertz. Existing
Windows returns raw QPC counts. API consumers must not assume both
platforms use the same QPC tick frequency. The Linux timer initializes
to zero, resets its frozen Stop value at Start, and rejects a null FPS
output pointer.

The new `jx_engine_timer_monotonic` regression compiles the ACTUAL
legacy `KTimer.cpp` and verifies Start/Stop/interval, monotonic elapsed
milliseconds, high-resolution ticks, Passed threshold and reset, plus
GetFPS before/after 1000 ms. This is a Linux source-slice acceptance
test, **not** certification of a Linux GameServer binary or multiplayer
runtime. Test uses real sleeping and coarse timing bands to limit jitter.

```bash
cmake -S legacy_jx/tests/linux_abi -B /tmp/jx-engine-linux -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/jx-engine-linux
ctest --test-dir /tmp/jx-engine-linux --output-on-failure
```

On Ubuntu x86_64 GCC 15.2 the expanded suite passed **3/3 ordinary**
and **3/3 with ASan+UBSan**. The timer C++ source itself compiles with
`-Wall -Wextra -Wpedantic -Werror`. Existing legacy `KNode.h`
compiler warnings remain in the separate node/list tests.
