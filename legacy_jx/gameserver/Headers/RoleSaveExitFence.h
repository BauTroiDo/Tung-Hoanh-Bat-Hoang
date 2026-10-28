#ifndef JX273_ROLE_SAVE_EXIT_FENCE_H
#define JX273_ROLE_SAVE_EXIT_FENCE_H
// A role must not be unlocked or transferred while a versioned save
// has an unresolved outcome. A retry timeout is NOT an ACK.
namespace JXRoleSaveExitFence {
static inline bool BlocksRelease(bool versionedSession,
                                  bool revisionLoaded,
                                  bool pendingSave) {
    return versionedSession && (pendingSave || !revisionLoaded);
}
}
#endif
