# JX1 3D native client login: AuthService / WorldGateway ticket contract

**Decision (2026-10-22):** The new JX1 3D project targets our own PC, Web
and Mobile clients. We **do not use Client V9**, and Bishop/Rainbow
handshake, opcode or packet compatibility is **not an acceptance gate** for
this new system. Legacy Bishop patches in prior PRs remain independent
historical work; they are not linked into this portable core.

## Target trust boundary

```text
New JX1 3D Client (PC/Web/Mobile)
  | HTTPS/TLS authenticated login; not implemented in this patch
  v
AuthService (verifies credential through trusted account provider)
  | Server-only ticket issuance API; stores one-use, short-lived ticket
  v
Client receives opaque bearer ticket over TLS
  | WSS/TLS World join with ticket
  v
WorldGateway (trusted, assigns nonzero connection ID)
  | Atomically consumes ticket using authoritative shared store
  | obtains (accountId, realmId, sessionGeneration, connectionId)
  v
WorldServer (authoritative session + AOI + movement)
  | GameServer receives trusted account/session context, NO password
```

## Implemented in this source PR (standalone, in one process)

The `mmo::portable::AuthTicketAuthority` class:
- Generates **32 bytes using the operating-system CSPRNG** (Windows
  BCryptGenRandom with system-preferred RNG; Linux getrandom).
  Encodes them as **64 lower-case hex characters**. Never substitutes
  std::rand, MT19937, timestamps or low-entropy tokens.
- Accepts issuance only from an **already trusted AuthService caller**;
  nonzero account/realm IDs and TTL **1–300 seconds** (30s default).
  This is *not* an API to accept credentials from arbitrary clients.
- Holds bounded pending tickets in a mutex-protected in-memory map.
  A ticket is bound to its realm and becomes unusable **atomically** on
  its first successful WorldGateway consume.
- `consume(ticket, realm, trustedConnectionId)` rejects zero connection,
  wrong realm, malformed ticket, expiry and replay. The resulting
  authenticated world session is bound to a **server-chosen**
  connectionId and unique generation (both must never come from clients).
- A new authenticated `issue()` for the same account revokes its old
  outstanding ticket and old active world generation (single-session
  policy). Old WorldServer connection contexts then fail `valid()`.
  Old `close()` cannot close a newer session.
- `revokeAccount()` handles authoritative logout/ban/revocation.
  `close()` releases a matching active session; expiry is checked
  with the server **monotonic** clock and issue prunes expired entries.
- On missing CSPRNG entropy, oversized TTL, account capacity reached,
  counter exhaustion and invalid requests, the call **fails closed**.
  Do not log or persist the raw bearer ticket. Do not reuse a token.

## IMPORTANT: NOT yet a deployable cross-process AuthService

The above works as a **portable single-process protocol/state primitive**.
It does **not** start a standalone AuthService, implement passwords/OAuth,
network TLS, persistence, distributed session uniqueness, or an endpoint
for GameServer. For an actual multi-process topology, AuthService and
WorldGateway **MUST** share a durable/authoritative store with an atomic
single-consume transaction and unique session generation (e.g. a
reviewed Redis Lua script/transaction or a database transaction).
A local in-memory authority in two different processes does **not**
work and must not be deployed as if it were a distributed protocol.

Additional production gates:
- Rate limit and lock out abusive attempts **at AuthService**, strong
  password hashing / account MFA, credential handling, auditing without
  recording bearer tokens or passwords.
- HTTPS/WSS and authenticated internal RPC/mTLS, short client ticket TTL,
  secret-safe transport/proxy configuration and replay/DoS testing.
- Per-world connection quotas, cross-realm/account policy, centralized
  revocation, data durability, HA failover and concurrent-disconnect
  race recovery.
- Bind trusted sessions to the WorldServer's AOI/movement owner lifecycle
  (do not trust client-supplied account IDs or session generation).
- Real PC, Web and Mobile client integration and E2E tests.

## Verification in this PR

Build/tests use existing `portable_core` CMake project and run
as an independent C++17 library on Windows x64 and Linux.
Negative/race tests cover issuance, TTL bounds, clock-controlled expiry,
bad/wrong realm/unknown/replayed token, wrong connection ID,
stale-session revocation, per-account singleton, capacity/expiry eviction,
one winner of 64 concurrent consumes (10 rounds), explicit logout and
owner-generation checks. No external account DB or network is contacted.

No Bishop/GameServer process, legacy source, Client V9, or server database
is changed. Never claim this is a production-grade AuthService from
unit tests alone.
