"""Static QA that every known character-removal entrypoint is gated.

This is a source-wiring regression, not a substitute for end-to-end login,
disconnect and restart testing. Native policy semantics are tested separately.
"""
from pathlib import Path

def check(ok,description):
    if not ok:
        raise AssertionError(description)
    print("PASS",description)

root=Path(__file__).resolve().parents[2]
core=(root/'gameserver/Sources/Core/Src/CoreServerShell.cpp').read_bytes()
gs=(root/'gameserver/Sources/MultiServer/GameServer/KSOServer.cpp').read_bytes()
player=(root/'gameserver/Sources/Core/Src/KPlayerSet.cpp').read_bytes()

def block_after(source,signature):
    check(source.count(signature)==1,signature.decode('ascii')+" unique")
    pos=source.index(signature)
    return source[pos:pos+4000]

core_entries=(
 (b'void CoreServerShell::ClientDisconnect(',b'PlayerSet.__PrepareRemove(nIndex);'),
 (b'void CoreServerShell::RemoveQuitingPlayer(',b'PlayerSet.RemoveQuiting(nIndex);'),
 (b'void CoreServerShell::mandatoryShutDownClient(',b'PlayerSet.PrepareRemove(nIndex,TRUE);')
)
for signature,removal in core_entries:
    block=block_after(core,signature)
    fence=b'if (IsRoleSaveReleaseBlocked(nIndex)) return;'
    check(fence in block and removal in block and block.index(fence)<block.index(removal),
          signature.decode('ascii')+" protects actual removal")

logouts=block_after(gs,b'void KSwordOnLineSever::PlayerLogoutGateway()')
fence=b'if (m_pCoreServerShell->IsRoleSaveReleaseBlocked(nIndex))'
check(fence in logouts,'GameServer logout is fenced')
check(logouts.index(fence)<logouts.index(b'IsPlayerLoginTimeOut(nIndex)'),
      'GameServer login-timeout branch fenced')
check(logouts.index(fence)<logouts.index(b'IsCharacterQuiting(nIndex)'),
      'GameServer quit branch fenced')

lowlevel=(
 (b'void KPlayerSet::PrepareRemove(',b'CheckMaps(nIndex,FALSE,isOnline);'),
 (b'void KPlayerSet::__PrepareRemove(',b'CheckMaps(nIndex,FALSE,FALSE);'),
 (b'void KPlayerSet::RemoveQuiting(',b'Player[nIndex].m_ItemList.RemoveAll();'),
 (b'void KPlayerSet::gm_PrepareRemove(',b'tagLeaveGame')
)
for signature,danger in lowlevel:
    block=block_after(player,signature)
    fence=b'JXRoleSaveExitFence::BlocksRelease('
    check(fence in block and danger in block and block.index(fence)<block.index(danger),
          signature.decode('ascii')+" guarded before mutation")

print('ALL_LOW_LEVEL_AND_PUBLIC_RELEASE_ENTRYPOINT_GATES_PASS')
