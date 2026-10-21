#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <type_traits>

// Includes the ACTUAL legacy JX Engine Linux compatibility header, not a mock.
#include "KWin32.h"

// Win64 uses LLP64; Linux x86_64 uses LP64. Portable wire-facing structs
// must retain Windows 32-bit LONG/DWORD, not Linux native 'long'.
static_assert(sizeof(DWORD) == 4, "DWORD must be 32 bits on Linux x86_64");
static_assert(sizeof(LONG) == 4, "LONG must be 32 bits on Linux x86_64");
static_assert(sizeof(WORD) == 2, "WORD must be 16 bits");
static_assert(sizeof(BYTE) == 1, "BYTE must be 8 bits");
static_assert(sizeof(RECT) == 16, "Windows RECT layout is four 32-bit values");
static_assert(sizeof(POINT) == 8, "Windows POINT layout is two 32-bit values");
static_assert(sizeof(GUID) == 16, "Windows GUID is sixteen bytes");
static_assert(offsetof(GUID, Data1) == 0, "GUID Data1 offset");
static_assert(offsetof(GUID, Data2) == 4, "GUID Data2 offset");
static_assert(offsetof(GUID, Data3) == 6, "GUID Data3 offset");
static_assert(offsetof(GUID, Data4) == 8, "GUID Data4 offset");
static_assert(sizeof(LPDWORD) == sizeof(void*), "LPDWORD is a pointer");

int main()
{
    DWORD value = 0xAABBCCDDu;
    LPDWORD pointer = &value;
    if (*pointer != 0xAABBCCDDu)
        return 1;

    GUID guid{};
    guid.Data1 = 0x01234567u;
    guid.Data2 = 0x89ABu;
    guid.Data3 = 0xCDEFu;
    guid.Data4[0] = 0x42;
    if (guid.Data1 != 0x01234567u || guid.Data4[0] != 0x42)
        return 2;

    RECT rect{};
    rect.left = -1;
    rect.right = 0x7FFFFFFF;
    if (rect.left != -1 || rect.right != 0x7FFFFFFF)
        return 3;

    std::printf("JX_LINUX_ABI_PASS DWORD=%zu LONG=%zu RECT=%zu POINT=%zu GUID=%zu\n",
        sizeof(DWORD), sizeof(LONG), sizeof(RECT), sizeof(POINT), sizeof(GUID));
    return 0;
}
