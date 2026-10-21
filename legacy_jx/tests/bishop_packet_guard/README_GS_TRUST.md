# Bishop V9 — explicit GameServer peer allowlist

The existing Bishop GameServer listener on port 56732 accepted every TCP
connection and created `CGameServer` objects before checking its remote
origin. This is an **offline source fix**, not a deployment to the running
Bishop process.

## Required configuration before rollout

In the `[Network]` section of Bishop.cfg, add:

```ini
[Network]
TrustedGameServerIPs=127.0.0.1,192.0.2.10
```

**192.0.2.10 is documentation-only**: replace it with the **actual, verified
remote IP of each legitimate GameServer** on the private network. Never
copy that example address to production. The option accepts a
comma-separated list of **literal IPv4 addresses**, at most 32; it
rejects hostnames, CIDR, wildcards, malformed addresses and empty lists.
The selected network config must be present BEFORE Bishop is started:
**no setting, blank setting or malformed list deliberately fails
`StartupNetwork()`** rather than silently allowing all peers. The
trusted list is loaded at startup; change it only during an approved
maintenance window. Include the key in the configuration provisioning
before deploying a new executable.

The layer below returns `CSocketAddress::GetAsString()` as
`IPv4 : port`, for example `127.0.0.1 : 35123`. The guard strictly
parses this format, converts each IPv4 octet to its numeric value and
compares it with the configured allowlist, independent of the remote port.
An unauthorized/invalid endpoint is disconnected **before** allocating
and activating a `CGameServer` object or accepting its packets.

## What this does NOT guarantee

- **Not cryptographic peer authentication.** Another process on an
  allowed host/IP may still connect. Follow with a mutually authenticated
  handshake or a trusted transport where original V9 wire compatibility
  can be demonstrated.
- Bishop still binds its GameServer listener to `INADDR_ANY`; firewall
  policy must separately restrict access to port 56732 to intended peers.
- This is an IP-source, callback-level gate. Lower-level IOCP framing,
  connection exhaustion and origin spoofing/host compromise are beyond it.
- No real PC V9 client login or GameServer registration E2E test has
  been performed, and no production executable was replaced.

## Regression tests

The portable no-network `test_bishop_gs_peer_guard.cpp` covers
valid addresses, the actual `IPv4 : port` grammar, zero/overflow ports,
fake IPs, embedded delimiters, unknown hosts, empty configuration,
partial/invalid lists, leading-zero aliases, and the 32-peer maximum.

```sh
cmake -S legacy_jx/tests/bishop_packet_guard -B /tmp/jx-bishop-trust -G Ninja
cmake --build /tmp/jx-bishop-trust
ctest --test-dir /tmp/jx-bishop-trust --output-on-failure
```

The preexisting packet length tests are retained. This is a source-slice
acceptance test and does not assert handler-level packet fuzz parity.
Do not enable it on a live server until the configuration and full
connection/login/role-list/GameServer compatibility gates pass.
