#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include "KWin32.h"
#include "KList.h"

static int check(bool yes, int code) { return yes ? 0 : code; }

int main()
{
    KList list;
    KNode first;
    KNode second;

    if (int result = check(list.IsEmpty() != FALSE, 1))
        return result;
    list.AddHead(&first);
    list.AddTail(&second);
    if (int result = check(list.GetNodeCount() == 2, 2))
        return result;
    if (int result = check(list.GetHead() == &first, 3))
        return result;
    if (int result = check(list.GetTail() == &second, 4))
        return result;

    if (int result = check(list.RemoveHead() == &first, 5))
        return result;
    if (int result = check(list.GetNodeCount() == 1, 6))
        return result;
    if (int result = check(list.RemoveTail() == &second, 7))
        return result;
    if (int result = check(list.IsEmpty() != FALSE, 8))
        return result;
    if (int result = check(list.GetNodeCount() == 0, 9))
        return result;

    std::puts("JX_ENGINE_KNODE_KLIST_LINUX_PASS");
    return 0;
}
