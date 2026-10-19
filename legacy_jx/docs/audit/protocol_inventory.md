# Network Protocol Inventory & Serialization Analysis

## 1. Transport Layer & Packet Framing
- **Transport Mechanism:** Windows IOCP (I/O Completion Ports) over TCP socket streams.
- **Framing Model:**
  - `PROTOCOL_MSG_TYPE`: `BYTE` opcode (`1 byte`).
  - Size Header: Packets are categorized into fixed-size micro-packets (`< 32 bytes`) or dynamic-size extended packets (`WORD m_wLength`).
  - Memory Alignment: `#pragma pack(push, 1)` byte-packed structs without compiler padding.
- **Protocol Versioning:** `#define KPROTOCOL_VERSION 1`.

---

## 2. Core Protocol Opcodes Matrix

### 2.1 Server-to-Client (`s2c_PROTOCOL`)
| Opcode Range | Purpose | Key Packets |
| :--- | :--- | :--- |
| `10 - 30` | Gateway & Authentication | `s2c_rolehjdkalritopq_getroleinfo_result`, `s2c_syncogjfkdnvhf_roleinfo_cipher` |
| `32 - 47` | Account Lifecycle | `s2c_accountfdfsglogin`, `s2c_gamelogin`, `s2c_accountlogout` |
| `48 - 63` | MultiServer & World Handshake | `s2c_querymapinfo`, `s2c_identitymapping`, `s2c_notifysvrip` |
| `64+` | World Entities & Simulation | NPC spawn/despawn, player sync, movement broadcast, skill cast, item drop |

### 2.2 Client-to-Server (`c2s_PROTOCOL`)
| Opcode Range | Purpose | Key Packets |
| :--- | :--- | :--- |
| `32 - 47` | Login & Character Select | `c2s_accountlogin`, `c2s_gamelogin_dkiqopjgurnmvbgc` |
| `48 - 63` | Server Transfer & Roles | `c2s__entergamegkjyiruqine`, `c2s_roleserver_getrolelist` |
| `64+` | Player Intent & Gameplay | `c2s_playerwalk`, `c2s_playerrun`, `c2s_playerskillcast`, `c2s_npctalk`, `c2s_trade` |

---

## 3. Movement & Entity Replication Analysis
- **Authoritative Model:** Server-authoritative. The client transmits destination coordinates $(X, Y)$ and action intent (walk/run/jump); the server validates path obstacles and broadcasts authoritative coordinates.
- **Coordinate System:**
  - SubWorld coordinates: $(Rx, Ry)$ region index + $(Dx, Dy)$ logic cell offsets within region ($32 \times 32$ logic pixels).
- **Client 3D Adapter Requirements (PR5 / PR8):**
  - To support Unity 3D / Isometric client, a protocol adapter must convert legacy 2D discrete cell packets into continuous vector transforms while retaining server combat authority.
