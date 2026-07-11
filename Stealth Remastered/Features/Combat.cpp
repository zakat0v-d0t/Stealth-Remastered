#include "main.h"

void Combat::Update()
{
	NoReload();
	AllowCBUG();
	RapidFire();
	FastCrosshair();
	NoRecoil();
	AutoScroll();
	AutoCBUG();
}

void Combat::NoReload()
{
	if (!g_Config.g_Combat.bNoReload)
		return;

	int iWeapon = FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_nType;
	if (iWeapon >= 22 && iWeapon <= 42 && FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_nAmmoInClip == 1)
		FindPlayerPed()->GiveWeapon((eWeaponType)iWeapon, 0, false);
}

void Combat::AllowCBUG()
{
	static bool bAllowCBug = false;
	if (g_Config.g_Combat.bAllowCBug)
	{
		if (g_Config.g_Combat.bCBugFreeze)
			pSAMP->patchWithSave(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugFreeze, "\xC3\x90", 2);
		if (g_Config.g_Combat.bCBugAnim)
			pSAMP->patchWithSave(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugAnim, "\xC3\x90", 2);
		if (g_Config.g_Combat.bCBugWeapon)
			pSAMP->patchWithSave(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugWeapon, "\xC3\x90", 2);
		if (g_Config.g_Combat.bCBugText)
			pSAMP->patchWithSave(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugText, "\xC3\x90", 2);
		bAllowCBug = true;
	}
	else if (bAllowCBug)
	{
		pSAMP->restoreOrigBytes(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugFreeze);
		pSAMP->restoreOrigBytes(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugAnim);
		pSAMP->restoreOrigBytes(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugWeapon);
		pSAMP->restoreOrigBytes(pSAMP->g_dwSAMP_Addr + pSAMP->offsets().dwCBugText);
		bAllowCBug = false;
	}
}

void Combat::RapidFire()
{
	if (!g_Config.g_Combat.bRapidFire)
		return;

	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "PYTHON_CROUCHFIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "PYTHON_FIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "PYTHON_FIRE_POOR", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "RIFLE_CROUCHFIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "RIFLE_FIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "RIFLE_FIRE_POOR", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "SHOTGUN_CROUCHFIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "SHOTGUN_FIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "SHOTGUN_FIRE_POOR", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "SILENCED_CROUCH_FIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "SILENCED_FIRE", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "TEC_crouchfire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "TEC_fire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "UZI_crouchfire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "UZI_fire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "UZI_fire_POOR", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "idle_rocket", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "Rocket_Fire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "run_rocket", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "walk_rocket", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "WALK_start_rocket", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "WEAPON_sniper", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "buddy_crouchfire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "buddy_fire", g_Config.g_Combat.fRapidFire);
	plugin::Command<plugin::Commands::SET_CHAR_ANIM_SPEED>(FindPlayerPed(), "buddy_fire_POOR", g_Config.g_Combat.fRapidFire);
}

void Combat::FastCrosshair()
{
	static bool bFastCrosshair = false;
	if (g_Config.g_Combat.bFastCrosshair)
	{
		pSecure->memcpy_safe((void*)0x0058E1D9, "\xEB", 1);
		bFastCrosshair = true;
	}
	else if (bFastCrosshair)
	{
		Memory::memcpy_safe((void*)0x0058E1D9, "\x74", 1);
		bFastCrosshair = false;
	}
}

void Combat::NoRecoil()
{
	if (g_Config.g_Combat.bNoRecoil)
		pSecure->memcpy_safe((void*)0x8D610F, "\xBE\x00\x00\x00\x00", 5);
	else Memory::memcpy_safe((void*)0x8D610F, "\xBE\x00\x00\x40\x3F", 5);
}

void Combat::WeaponSwitcher()
{
	if (g_Config.g_Combat.bWeaponSwitcher && !FindPlayerVehicle(-1, false))
	{
		for (size_t i = 0; i < g_Config.g_Hotkeys.iWeaponSwitch.size(); i++)
		{
			if (isKeyPressed(g_Config.g_Hotkeys.iWeaponSwitch[i]))
			{
				if (g_Config.g_Combat.bFastSwitch)
					FindPlayerPed()->SetCurrentWeapon(i + 2);
				else *(DWORD*)0xB7CDBC = i + 2;
			}
		}
	}
}

void Combat::AutoScroll()
{
	if (g_Config.g_Combat.bAutoScroll)
	{
		static int iSlot = 0;
		static int iState = 0;

		if (pSAMP->getPlayers()->pLocalPlayer->onFootData.stSampKeys.keys_aim && FindPlayerPed()->m_nActiveWeaponSlot == 2) {
			iState = 1;
		}
		else if (iState == 2)
		{
			FindPlayerPed()->SetCurrentWeapon(iSlot);
			iState = 0;
		}
		else if (iState == 1)
		{
			iSlot = FindPlayerPed()->m_nActiveWeaponSlot;
			FindPlayerPed()->m_nActiveWeaponSlot = 0;
			iState = 2;
		}
	}
}

void Combat::AutoCBUG()
{
	if (!g_Config.g_Combat.bAutoCBug)
		return;

	if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 24)
		return;

	static int iStep = 0;
	if (FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_nAmmoInClip && GetAsyncKeyState(VK_LBUTTON) && GetAsyncKeyState(VK_RBUTTON))
	{
		pKeyHook->g_GameKeyState[17] = { 0, true };
		static ULONGLONG ulWait = 0;
		static ULONGLONG ulTick = 0;
		if (GetTickCount64() - ulTick > ulWait)
		{
			ulTick = GetTickCount64();
			switch (iStep)
			{
			case 0:
			{
				pKeyHook->g_GameKeyState[17] = { 0xFF, true };
				ulWait = g_Config.g_Combat.iCBugDelay[1];
				break;
			}
			case 1:
			{
				pKeyHook->g_GameKeyState[6] = { 0, true };
				pKeyHook->g_GameKeyState[18] = { 0xFF, true };
				ulWait = g_Config.g_Combat.iCBugDelay[0];
				break;
			}
			}

			if (iStep++ > 1)
				iStep = 0;
		}
	}
	else iStep = 0;
}