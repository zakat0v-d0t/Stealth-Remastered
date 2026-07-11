#include "main.h"

CSAMP* pSAMP;

// ============================================================
// SA-MP 0.3.7-R1 offset table
// ============================================================
const SAMPOffsets g_Offsets_R1 =
{
	// Core pointers
	0x21A0F8,	// dwSampInfo
	0x21A10C,	// dwMiscInfo
	0x21A0E4,	// dwChatInfo
	0x21A0E8,	// dwInputInfo
	0x21A0EC,	// dwKillInfo
	0x216378,	// dwColorOffset

	// Functions
	0x064010,	// dwAddToChatWnd
	0x09BD30,	// dwToggleCursor
	0x09BC10,	// dwCursorUnlockActorCam
	0x065C60,	// dwSendCommand
	0x0057F0,	// dwSay
	0x0661B0,	// dwWeaponSpriteID
	0x05DB40,	// dwWndProc

	// Aimbot hooks
	0x0B05A0,	// dwFireInstantHit
	0x0A0BB0,	// dwAddBullet

	// RakNet
	0x030B30,	// dwRPC
	0x0307F0,	// dwSend
	0x03A560,	// dwRPCRestore
	0x033DC0,	// dwSendRestore1
	0x037490,	// dwSendRestore2

	// Anti-cheat
	0x099230,	// dwAntiCheat

	// CBug
	0x0168E0,	// dwCBugFreeze
	0x016FA0,	// dwCBugAnim
	0x015530,	// dwCBugWeapon
	0x015F40,	// dwCBugText

	// Visual offsets
	0x068B0C,	// dwHealthBarColor
	0x068B33,	// dwHealthBarBG
	0x068DD5,	// dwArmorBarColor
	0x068E00,	// dwArmorBarBG
	0x09D9D0,	// dwFPSUnlock
};

// ============================================================
// SA-MP 0.3.7-R5-1 offset table
// Verified offsets marked with [V], estimated with [E]
// ============================================================
const SAMPOffsets g_Offsets_R5 =
{
	// Core pointers [V] - verified via blast.hk / ugbase
	0x26EB94,	// dwSampInfo       [V]
	0x26EBAC,	// dwMiscInfo       [V]
	0x26EB80,	// dwChatInfo       [V]
	0x26EB84,	// dwInputInfo      [V]
	0x26EB88,	// dwKillInfo       [V]
	0x26AE14,	// dwColorOffset    [E] data section delta +0x54A9C

	// Functions
	0x069900,	// dwAddToChatWnd   [V]
	0x0A06F0,	// dwToggleCursor   [V]
	0x0A05D0,	// dwCursorUnlockActorCam [E] 0x120 before ToggleCursor
	0x06B5C0,	// dwSendCommand    [E] chat section delta
	0x005860,	// dwSay            [E]
	0x06BAA0,	// dwWeaponSpriteID [E] chat section delta
	0x063430,	// dwWndProc        [E]

	// Aimbot hooks
	0x0B5090,	// dwFireInstantHit [E]
	0x0A5610,	// dwAddBullet      [E]

	// RakNet [V] RPC/Send verified, restore computed with same delta
	0x034620,	// dwRPC            [V]
	0x0342E0,	// dwSend           [V]
	0x03E050,	// dwRPCRestore     [E] RakNet delta +0x3AF0
	0x0378B0,	// dwSendRestore1   [E] RakNet delta +0x3AF0
	0x03AF80,	// dwSendRestore2   [E] RakNet delta +0x3AF0

	// Anti-cheat
	0x09DBF0,	// dwAntiCheat      [E]

	// CBug [E] - estimated delta ~0x3C00
	0x01A4E0,	// dwCBugFreeze     [E]
	0x01ABA0,	// dwCBugAnim       [E]
	0x019130,	// dwCBugWeapon     [E]
	0x019B40,	// dwCBugText       [E]

	// Visual offsets [E] - chat section delta +0x58F0
	0x06E3FC,	// dwHealthBarColor [E]
	0x06E423,	// dwHealthBarBG    [E]
	0x06E6C5,	// dwArmorBarColor  [E]
	0x06E6F0,	// dwArmorBarBG     [E]
	0x0A2390,	// dwFPSUnlock      [E]
};

// ============================================================
// Version detection via PE image size
// ============================================================
eSAMPVersion CSAMP::detectVersion()
{
	MODULEINFO mi;
	if (GetModuleInformation(GetCurrentProcess(), (HMODULE)g_dwSAMP_Addr, &mi, sizeof(mi)))
	{
		// R5-1 binary is significantly larger than R1
		if (mi.SizeOfImage >= 0x400000)
			return SAMP_VERSION_R5;
	}
	return SAMP_VERSION_R1;
}

// ============================================================
// Save/restore original bytes (version-independent)
// ============================================================
void CSAMP::saveOrigBytes(DWORD addr, size_t size)
{
	if (m_savedBytes.find(addr) == m_savedBytes.end())
	{
		std::vector<BYTE> bytes(size);
		memcpy(bytes.data(), (void*)addr, size);
		m_savedBytes[addr] = bytes;
	}
}

void CSAMP::restoreOrigBytes(DWORD addr)
{
	auto it = m_savedBytes.find(addr);
	if (it != m_savedBytes.end())
		Memory::memcpy_safe((void*)addr, (char*)it->second.data(), it->second.size());
}

void CSAMP::patchWithSave(DWORD addr, const char* bytes, size_t size)
{
	saveOrigBytes(addr, size);
	pSecure->memcpy_safe((void*)addr, bytes, size);
}

// ============================================================
// Initialization
// ============================================================
bool CSAMP::tryInit()
{
	const SAMPOffsets& off = *m_pOffsets;

	g_SAMP = *(stSAMP**)(g_dwSAMP_Addr + off.dwSampInfo);
	if (g_SAMP == nullptr)
		return false;

	g_Chat = *(stChatInfo**)(g_dwSAMP_Addr + off.dwChatInfo);
	if (g_Chat == nullptr)
		return false;

	g_Input = *(stInputInfo**)(g_dwSAMP_Addr + off.dwInputInfo);
	if (g_Input == nullptr)
		return false;

	g_DeathList = *(stKillInfo**)(g_dwSAMP_Addr + off.dwKillInfo);
	if (g_DeathList == nullptr)
		return false;

	if (g_SAMP->pRakClientInterface == nullptr)
		return false;

	g_Vehicles = g_SAMP->pPools->pVehicle;
	g_Players = g_SAMP->pPools->pPlayer;

	pSecure->memcpy_safe((void*)0x584CFF, "\x90\x90\x90\x90\x90", 5);
	pSecure->memcpy_safe((void*)0x584BDD, "\x90\x90\x90\x90\x90", 5);
	pSecure->memcpy_safe((void*)0x584C2A, "\x90\x90\x90\x90\x90", 5);
	patchWithSave(g_dwSAMP_Addr + off.dwAntiCheat, "\xC3", 1);

	g_dwSAMPCAC_Addr = (DWORD)LoadLibraryA("!sampcac_client.asi");

	return true;
};

void CSAMP::addMessageToChat(D3DCOLOR dwColor, const char* szMsg, ...)
{
	if (g_Chat == nullptr) return;
	if (szMsg == NULL) return;

	va_list ap;
	char tmp[512];
	memset(tmp, 0, 512);
	va_start(ap, szMsg);
	vsnprintf(tmp, sizeof(tmp) - 1, szMsg, ap);
	va_end(ap);

	return ((void(__thiscall*) (const void*, int, char*, char*, DWORD, DWORD)) (g_dwSAMP_Addr + m_pOffsets->dwAddToChatWnd))((void*)g_Chat, 8, tmp, NULL, dwColor, 0x00);;
}

void CSAMP::addSayToChatWindow(char* szText, ...)
{
	if (g_Input == NULL) return;
	if (szText == NULL) return;

	va_list ap;
	char tmp[128];
	memset(tmp, 0, 128);
	va_start(ap, szText);
	vsprintf(tmp, szText, ap);
	va_end(ap);

	if (tmp[0] == '/')
		((void(__thiscall*) (void*, char*))(g_dwSAMP_Addr + m_pOffsets->dwSendCommand))(g_Input, tmp);
	else ((void(__thiscall*) (void*, char*))(g_dwSAMP_Addr + m_pOffsets->dwSay))(g_Players->pLocalPlayer, tmp);
}

void CSAMP::toggleSAMPCursor(int iToggle)
{
	if (g_SAMP == NULL) return;
	if (g_Input->iInputEnabled) return;

	void* pMiscInfo = *(void**)(g_dwSAMP_Addr + m_pOffsets->dwMiscInfo);
	((void(__thiscall*)(void*, int, bool))(g_dwSAMP_Addr + m_pOffsets->dwToggleCursor))(pMiscInfo, iToggle ? 3 : 0, !iToggle);
	if (!iToggle)
		((void(__thiscall*)(void*))(g_dwSAMP_Addr + m_pOffsets->dwCursorUnlockActorCam))(pMiscInfo);
}

bool CSAMP::isPlayerStreamed(const uint16_t playerID)
{
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
		return false;

	return true;
}

const char* CSAMP::getPlayerName(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;

	if (iPlayerID == g_Players->sLocalPlayerID)
	{
		if (g_Players->iLocalPlayerNameAllocated <= 0xF)
			return g_Players->szLocalPlayerName;
		return g_Players->pszLocalPlayerName;
	}

	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;

	if (g_Players->pRemotePlayer[iPlayerID]->iNameAllocated <= 0xF)
		return g_Players->pRemotePlayer[iPlayerID]->szPlayerName;

	return g_Players->pRemotePlayer[iPlayerID]->pszPlayerName;
}

D3DCOLOR CSAMP::getPlayerColor(int iPlayerID)
{
	D3DCOLOR* dwColor;
	if (iPlayerID < 0 || iPlayerID >= (SAMP_MAX_PLAYERS + 3))
		return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);

	switch (iPlayerID)
	{
	case (SAMP_MAX_PLAYERS):
		return 0xFF888888;

	case (SAMP_MAX_PLAYERS + 1):
		return 0xFF0000AA;

	case (SAMP_MAX_PLAYERS + 2):
		return 0xFF63C0E2;
	}

	dwColor = (D3DCOLOR*)((uint8_t*)g_dwSAMP_Addr + m_pOffsets->dwColorOffset);
	return D3DCOLOR_RGBA(dwColor[iPlayerID] >> 8, dwColor[iPlayerID] >> 16, dwColor[iPlayerID] >> 24, 255);
}

bool CSAMP::isVehicleStreamed(uint16_t vehicleID)
{
	if (g_Vehicles->iIsListed[vehicleID] != 1)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
		return false;
	return true;
}

int CSAMP::getNearestPlayer(bool bTeamProtect)
{
	int iPlayerID = -1;
	float fNearestDistance = -1.0f;

	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		if (g_Players->pRemotePlayer[i] == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
			continue;
		if (!CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle)->IsAlive())
			continue;
		if (bTeamProtect && getPlayerColor(i) == getPlayerColor(g_Players->sLocalPlayerID))
			continue;

		float fDistance = Math::vect3_dist(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], &g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
		if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
		{
			iPlayerID = i;
			fNearestDistance = fDistance;
		}
	}
	return iPlayerID;
}

int CSAMP::getNearestVehicle()
{
	int iVehicleID = -1;
	float fNearestDistance = -1.0f;

	for (int i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[i] == NULL)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle == NULL)
			continue;

		float fDistance = Math::vect3_dist(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], &g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle->base.matrix[12]);
		if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
		{
			fNearestDistance = fDistance;
			iVehicleID = i;
		}
	}
	return iVehicleID;
}

const char* CSAMP::getWeaponSpriteID(char szWeapon)
{
	return ((const char*(__thiscall*)(stKillInfo*, char))(g_dwSAMP_Addr + m_pOffsets->dwWeaponSpriteID))(pSAMP->getDeathList(), szWeapon);
}

float fWeaponDamage[55] =
{
	1.0, // 0 - Fist
	1.0, // 1 - Brass knuckles
	1.0, // 2 - Golf club
	1.0, // 3 - Nitestick
	1.0, // 4 - Knife
	1.0, // 5 - Bat
	1.0, // 6 - Shovel
	1.0, // 7 - Pool cue
	1.0, // 8 - Katana
	1.0, // 9 - Chainsaw
	1.0, // 10 - Dildo
	1.0, // 11 - Dildo 2
	1.0, // 12 - Vibrator
	1.0, // 13 - Vibrator 2
	1.0, // 14 - Flowers
	1.0, // 15 - Cane
	82.5, // 16 - Grenade
	0.0, // 17 - Teargas
	1.0, // 18 - Molotov
	9.9, // 19 - Vehicle M4 (custom)
	46.2, // 20 - Vehicle minigun (custom)
	0.0, // 21
	8.25f, // 22 - Colt 45
	13.200001f, // 23 - Silenced
	46.200001f, // 24 - Deagle
	49.500004f,//3.3, // 25 - Shotgun
	49.500004f,//3.3, // 26 - Sawed-off
	39.600002f,//4.95, // 27 - Spas
	6.6f, // 28 - UZI
	8.25f, // 29 - MP5
	9.900001f, // 30 - AK47
	9.900001f, // 31 - M4
	6.6f, // 32 - Tec9
	24.750002f, // 33 - Cuntgun
	41.25f, // 34 - Sniper
	82.5, // 35 - Rocket launcher
	82.5, // 36 - Heatseeker
	1.0, // 37 - Flamethrower
	46.200001f, // 38 - Minigun
	82.5, // 39 - Satchel
	0.0, // 40 - Detonator
	0.33, // 41 - Spraycan
	0.33, // 42 - Fire extinguisher
	0.0, // 43 - Camera
	0.0, // 44 - Night vision
	0.0, // 45 - Infrared
	0.0, // 46 - Parachute
	0.0, // 47 - Fake pistol
	2.64, // 48 - Pistol whip (custom)
	9.9, // 49 - Vehicle
	330.0, // 50 - Helicopter blades
	82.5, // 51 - Explosion
	1.0, // 52 - Car park (custom)
	1.0, // 53 - Drowning
	165.0 // 54 - Splat
};

float fWeaponRange[39] =
{
	0.0, // 0 - Fist
	0.0, // 1 - Brass knuckles
	0.0, // 2 - Golf club
	0.0, // 3 - Nitestick
	0.0, // 4 - Knife
	0.0, // 5 - Bat
	0.0, // 6 - Shovel
	0.0, // 7 - Pool cue
	0.0, // 8 - Katana
	0.0, // 9 - Chainsaw
	0.0, // 10 - Dildo
	0.0, // 11 - Dildo 2
	0.0, // 12 - Vibrator
	0.0, // 13 - Vibrator 2
	0.0, // 14 - Flowers
	0.0, // 15 - Cane
	0.0, // 16 - Grenade
	0.0, // 17 - Teargas
	0.0, // 18 - Molotov
	90.0, // 19 - Vehicle M4 (custom)
	75.0, // 20 - Vehicle minigun (custom)
	0.0, // 21
	35.0, // 22 - Colt 45
	35.0, // 23 - Silenced
	35.0, // 24 - Deagle
	40.0, // 25 - Shotgun
	35.0, // 26 - Sawed-off
	40.0, // 27 - Spas
	35.0, // 28 - UZI
	45.0, // 29 - MP5
	70.0, // 30 - AK47
	90.0, // 31 - M4
	35.0, // 32 - Tec9
	100.0, // 33 - Cuntgun
	320.0, // 34 - Sniper
	0.0, // 35 - Rocket launcher
	0.0, // 36 - Heatseeker
	0.0, // 37 - Flamethrower
	75.0  // 38 - Minigun
};