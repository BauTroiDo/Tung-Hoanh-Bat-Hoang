# Client-3D-2 offline PAK bootstrap — **source-only candidate**

**Roadmap:** JX3D M1/GAME-02.  
**State:** Offline C# component tests passed on Windows. **Not integrated into `Game.exe`**.

This package is deliberately separate from `portable_core` (C++17) and from the
deployed GameServer. It is a reference implementation that can be wired into
an **authorized Unity C# source project** and then rebuilt with IL2CPP.
It is not a DLL injection, binary patch, replacement Bishop server, or
new local HTTP/CDN server.

## Supported input

Client path: `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update`.

The installed versioned PACK v1 set is:

| PAK | Indexed resources | Verified in Windows test |
| --- | ---: | --- |
| `script.v110.pak` | 407 | all CRC32 |
| `settings.v149.pak` | 312 | all CRC32 |
| `maps.v111.pak` | 379 | all CRC32 |
| `UI.v102.pak` | 258 | all CRC32 |
| `spr.v113.pak` | 9,408 | all CRC32 |
| **Total** | **10,764** | **10,764/10,764** |

These counts are **specific to this installed version** and the gate fails
closed if a later version has changed membership. No customer game assets,
PAK binaries, encrypted material, credentials or character data are in Git.

`PakFile` indexes, decompresses and validates member length and CRC32;
`LocalPakResolver` maps logical resource paths to one of five installed
archives; `OfflinePakBootGate` requires early UI, Settings, Maps, Script
and SPR resources before declaring `Ready`. The implementation never makes
a network request and does not silently fall back to the public CDN.

## How to rerun tests on the authorized Windows machine

Use the .NET Framework C# compiler at
`C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe`.
The commands below run **standalone** console programs, not Unity:

```powershell
$root = "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update"
$csc = "C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe"
$src  = "client_offline_pak\src"
$test = "client_offline_pak\tests"
& $csc /nologo /optimize+ /target:exe /out:OfflinePakSmoke.exe /reference:System.IO.Compression.dll "$src\PakFile.cs" "$test\OfflinePakSmoke.cs"
& .\OfflinePakSmoke.exe $root

& $csc /nologo /optimize+ /target:exe /out:OfflinePakVerifyAll.exe /reference:System.IO.Compression.dll "$src\PakFile.cs" "$test\OfflinePakVerifyAll.cs"
& .\OfflinePakVerifyAll.exe $root

& $csc /nologo /optimize+ /target:exe /out:TestLocalPakResolver.exe /reference:System.IO.Compression.dll "$src\PakFile.cs" "$src\LocalPakResolver.cs" "$test\TestLocalPakResolver.cs"
& .\TestLocalPakResolver.exe $root

& $csc /nologo /optimize+ /target:exe /out:TestOfflinePakBootGate.exe /reference:System.IO.Compression.dll "$src\PakFile.cs" "$src\LocalPakResolver.cs" "$src\OfflinePakBootGate.cs" "$test\TestOfflinePakBootGate.cs"
& .\TestOfflinePakBootGate.exe $root
```

Local results on 2026-10-23: **15/15 smoke**, **10,764/10,764
CRC**, **10/10 resolver**, and **19/19 boot-gate** checks passed.

## Native Unity integration is still blocked

The selected client is Unity 6000.3.9f1 IL2CPP and ships compiled
`Game.exe` / `GameAssembly.dll`; its original Unity C# project was not
located in the working directories. Placing these C# files or a standalone
EXE next to the installed game **does not integrate them** into IL2CPP.

For a first-party Unity rebuild:

1. Initialize `OfflinePakBootGate.Prepare(<update>/paks)` on the
   `LoadResource` boot path **before** `WarmupBootAssetsAsync` and the first
   `UI/loadtip.ini` request.
2. Adapt `LoadResource.TryLoadPakBytes` to use
   `OfflinePakBootGate.TryLoadPakBytes`, preserving existing game
   file decoders and resource-name semantics.
3. Propagate a deterministic `Ready`/error result to the game's
   initialization state machine. Disable public CDN fallback in offline
   mode; do **not** fake `config.ini` or `version.json`.
4. Validate a fresh **real Game.exe** boot by confirming all five PAKs
   are actually registered, boot UI assets render, and no unauthorized
   outbound connection occurs.
5. Only then assess compatible Bishop wire ABI/role list and WorldSync
   in the isolated DEV lane. Present LAN test port **14182 is a synthetic
   handshake probe, not production Bishop 56722**.

The historical Unity log only prints `[Pak] Cache : ...` for each of the
five assets after a CDN fetch failure; it does **not** print a completed
`Loaded 5 pak(s)` or confirm a live login. This PR must remain **draft**
until native runtime integration and new-client authentication are tested.

## Production boundaries

Do not restart `gs001.exe`, alter character DB, overwrite original
`level0` / `global-metadata.dat`, replace script/settings authoritatively,
or merge this package into a runtime binary without explicit acceptance.
