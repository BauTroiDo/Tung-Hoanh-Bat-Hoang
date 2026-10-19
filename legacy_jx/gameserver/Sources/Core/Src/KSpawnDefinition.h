#ifndef KSPAWNDEFINITION_H
#define KSPAWNDEFINITION_H

#include "KEngine.h"
#include <string>
#include <vector>
#include <stdint.h>

#pragma pack(push, 1)
// Tách dữ liệu tĩnh của NPC (SpawnDefinition) khỏi đối tượng runtime KNpc (8536 bytes)
// Cấu trúc gọn nhẹ, bất biến khi nạp từ file map .dat
struct KSpawnDefinition
{
	uint64_t		uSpawnId;			// Stable 64-bit spawn ID: (mapId << 32) | (regionIdx << 16) | localNpcIndex
	int				nSubWorld;			// Map / SubWorld Index
	int				nRegionIdx;			// Region index trong SubWorld
	int				nTemplateID;		// Template ID trong NpcS.txt
	int				nPositionX;			// Tọa độ MPS X
	int				nPositionY;			// Tọa độ MPS Y
	short			nLevel;				// Level của quái
	short			shKind;				// Loại NPC (3 = quái, khác = đối thoại/thương nhân)
	unsigned char	cSeries;			// Ngũ hành (0: Kim, 1: Mộc, 2: Thủy, 3: Hỏa, 4: Thổ)
	unsigned char	cCamp;				// Trận doanh
	bool			bSpecialNpc;		// Quái đặc biệt / Hoàng kim
	bool			bIsDead;			// Trạng thái đã chết / chờ hồi sinh
	int				nCurLife;			// Sinh lực hiện tại khi dormant
	DWORD			dwNextRespawnTick;	// Thời điểm hồi sinh (tick clock)
	char			szName[32];			// Tên NPC
	char			szScript[64];		// Tên file Lua script (rút gọn từ 128 bytes)
	char			nNDropFile[64];		// Normal drop rate script
	char			nGDropFile[64];		// Golden drop rate script
	int				nRuntimeNpcIndex;	// Reference tới slot KNpc trong Npc[] (0 nếu chưa spawn/dormant)
};
#pragma pack(pop)

// Feature Flag điều khiển PR1 (Static/Runtime Split)
extern BOOL g_bSpawnDefinitionSplitEnabled;

// Feature Flag điều khiển PR3 (NPC Dynamic Activation & Region Streaming)
// Mặc định = FALSE (OFF) để đảm bảo an toàn tuyệt đối và tính tương thích
extern BOOL g_bNpcActivationEnabled;

// Inline helper tạo stable spawn ID 64-bit bất biến
inline uint64_t MakeStableSpawnId(uint32_t mapId, uint16_t regionIdx, uint16_t localNpcIdx)
{
	return ((uint64_t)mapId << 32) | ((uint64_t)regionIdx << 16) | (uint64_t)localNpcIdx;
}

#endif // KSPAWNDEFINITION_H
