#include "../../legacy_jx/gameserver/Sources/Core/Src/Jx3dM1DevAoiHooks.h"
#include <iostream>
#if defined(_SERVER) && defined(JX3D_M1_DEV_AOI)
#error "This target proves that DEV hooks remain disabled by default"
#endif
int main() {
    // These arguments must not even be evaluated in normal builds.
    int evaluated=0;
    JX3D_M1_DEV_AOI_EMIT_SPAWN(++evaluated);
    JX3D_M1_DEV_AOI_EMIT_MOVE(++evaluated);
    JX3D_M1_DEV_AOI_EMIT_REMOVE(++evaluated,++evaluated);
    JX3D_M1_DEV_AOI_EMIT_TICK(++evaluated);
    if (evaluated != 0) return 1;
    std::cout << "LEGACY_JX_DEV_HOOK_DEFAULT_OFF_PASS\n";
    return 0;
}
