#include "stdafx.h"
#include "CCRC32.H"
#include "MapManager.h"
#include "CustomGlow.h"
#include "CustomItem.h"
#include "ItemOption.h"
#include "CustomMonster.h"
#include "CustomBow.h"
#include "CustomWing.h"
#include "ItemPosition.h"
#include "MapFog.h"

struct MAIN_FILE_INFO
{
	char CustomerName[32];
	BYTE LauncherType;
	char LauncherName[32];
	char IpAddress[32];
	WORD IpAddressPort;
	char ClientSerial[17];
	char ClientVersion[8];
	BYTE MultiInstanceBlock;
	char WindowName[128];
	char ScreenShotPath[256];
	BYTE DisableResets;
	BYTE DisableGrandResets;
	char ClientName[128];
	char PluginName[128];
	DWORD ClientCRC32;
	DWORD PluginCRC32;
	DWORD ReconnectTime;
	BYTE HealthBarType;
	BYTE EnableMoveList;
	BYTE EnableSpecialCharacters;
	MAP_MANAGER_INFO MapManager[MAX_MAPS];
	CUSTOM_GLOW_INFO CustomGlowInfo[MAX_ITEM];
	CUSTOM_ITEM_INFO CustomItemInfo[MAX_ITEM];
	CUSTOM_BOW_INFO CustomBowInfo[MAX_CUSTOM_BOW];
	CUSTOM_WING_INFO CustomWingInfo[MAX_CUSTOM_WING];
	ITEM_OPTION_INFO ItemOptionInfo[MAX_ITEM_OPTION_INFO];
	CUSTOM_MONSTER_INFO CustomMonsterInfo[MAX_MONSTER];
	ITEM_POSITION_INFO ItemPositionInfo[MAX_ITEM];
	MAP_FOG_INFO MapFogInfo[MAX_MAPS];
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "es_ES.1252");

	SetConsoleOutputCP(1252);

	MAIN_FILE_INFO info;

	memset(&info, 0, sizeof(info));

	/*****************************************************************/
	/********************* Read .ini file values *********************/
	/*****************************************************************/

	GetPrivateProfileString("Licence", "CustomerName", "", info.CustomerName, sizeof(info.CustomerName), ".\\MainInfo.ini");

	info.LauncherType = GetPrivateProfileInt("Launcher", "LauncherType", 0, ".\\MainInfo.ini");
	GetPrivateProfileString("Launcher", "LauncherName", "XTLauncherExecutable", info.LauncherName, sizeof(info.LauncherName), ".\\MainInfo.ini");

	GetPrivateProfileString("Connection", "IpAddress", "", info.IpAddress, sizeof(info.IpAddress), ".\\MainInfo.ini");
	info.IpAddressPort = GetPrivateProfileInt("Connection", "IpAddressPort", 44405, ".\\MainInfo.ini");
	GetPrivateProfileString("Connection", "ClientSerial", "", info.ClientSerial, sizeof(info.ClientSerial), ".\\MainInfo.ini");
	GetPrivateProfileString("Connection", "ClientVersion", "", info.ClientVersion, sizeof(info.ClientVersion), ".\\MainInfo.ini");

	info.MultiInstanceBlock = GetPrivateProfileInt("ClientInfo", "BlockMultiInstance", 0, ".\\MainInfo.ini");
	GetPrivateProfileString("ClientInfo", "WindowName", "", info.WindowName, sizeof(info.WindowName), ".\\MainInfo.ini");
	GetPrivateProfileString("ClientInfo", "ScreenShotPath", "", info.ScreenShotPath, sizeof(info.ScreenShotPath), ".\\MainInfo.ini");
	info.DisableResets = GetPrivateProfileInt("ClientInfo", "DisableResets", 0, ".\\MainInfo.ini");
	info.DisableGrandResets = GetPrivateProfileInt("ClientInfo", "DisableGrandResets", 0, ".\\MainInfo.ini");

	GetPrivateProfileString("CheckCRC", "ClientName", "", info.ClientName, sizeof(info.ClientName), ".\\MainInfo.ini");
	GetPrivateProfileString("CheckCRC", "PluginName", "", info.PluginName, sizeof(info.PluginName), ".\\MainInfo.ini");

	/*================================================================*/

	info.ReconnectTime = GetPrivateProfileInt("Systems", "ReconnectTime", 0, ".\\MainInfo.ini");

	info.HealthBarType = GetPrivateProfileInt("Systems", "HealthBarType", 0, ".\\MainInfo.ini");

	info.EnableMoveList = GetPrivateProfileInt("Systems", "EnableMoveList", 0, ".\\MainInfo.ini");

	info.EnableSpecialCharacters = GetPrivateProfileInt("Systems", "EnableSpecialCharacters", 0, ".\\MainInfo.ini");

	/*****************************************************************/
	/************************ Read .txt files ************************/
	/*****************************************************************/

	gMapManager.Load("MapManager.txt");

	gCustomGlow.Load("CustomGlow.txt");

	gCustomItem.Load("CustomItem.txt");

	gCustomBow.Load("CustomBow.txt");

	gCustomWing.Load("CustomWing.txt");

	gItemOption.Load("ItemOption.txt");

	gCustomMonster.Load("CustomMonster.txt");
	
	gItemPosition.Load("ItemPosition.txt");

	gMapFog.Load("MapFog.txt");

	/*****************************************************************/
	/*********************** Load struct files ***********************/
	/*****************************************************************/

	memcpy(info.MapManager, gMapManager.m_MapManager, sizeof(info.MapManager));

	memcpy(info.CustomGlowInfo, gCustomGlow.m_CustomGlowInfo, sizeof(info.CustomGlowInfo));

	memcpy(info.CustomItemInfo, gCustomItem.m_CustomItemInfo, sizeof(info.CustomItemInfo));

	memcpy(info.CustomBowInfo, gCustomBow.m_CustomBowInfo, sizeof(info.CustomBowInfo));

	memcpy(info.CustomWingInfo, gCustomWing.m_CustomWingInfo, sizeof(info.CustomWingInfo));

	memcpy(info.ItemOptionInfo, gItemOption.m_ItemOptionInfo, sizeof(info.ItemOptionInfo));

	memcpy(info.CustomMonsterInfo, gCustomMonster.m_CustomMonsterInfo, sizeof(info.CustomMonsterInfo));

	memcpy(info.ItemPositionInfo, gItemPosition.m_ItemPositionInfo, sizeof(info.ItemPositionInfo));

	memcpy(info.MapFogInfo, gMapFog.m_MapFog, sizeof(info.MapFogInfo));

	/*=================================================================*/

	char _path_[MAX_PATH] = { 0 };

	CCRC32 CRC32;

	wsprintf(_path_, ".\\Client\\%s", info.ClientName);

	if (CRC32.FileCRC(_path_, &info.ClientCRC32, 1024) == 0)
	{
		info.ClientCRC32 = 0;
	}

	wsprintf(_path_, ".\\Client\\%s", info.PluginName);

	if (CRC32.FileCRC(_path_, &info.PluginCRC32, 1024) == 0)
	{
		info.PluginCRC32 = 0;
	}

	BYTE XorKey[20] = { 0x9E, 0x98, 0x12, 0x2B, 0xB6, 0x76, 0xF6, 0x5B, 0x76, 0x4D, 0x08, 0xF2, 0xAB, 0xDF, 0x91, 0xB6, 0x4C, 0x1C, 0x93, 0x2F };

	for (int n = 0; n < sizeof(MAIN_FILE_INFO); n++)
	{
		((BYTE*)&info)[n] ^= (BYTE)(XorKey[n % 20] ^ LOBYTE(n));

		((BYTE*)&info)[n] -= (BYTE)(XorKey[n % 20] ^ HIBYTE(n));

		((BYTE*)&info)[n] += (BYTE)(XorKey[n % 20] ^ HIBYTE(n));
	}

	HANDLE file = CreateFile(".\\Client\\Data\\Local\\ClientInfo.bmd", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		std::cout << "ERROR: Couldn't create the file." << std::endl;

		system("pause");

		return 1;
	}

	DWORD OutSize = 0;

	if (WriteFile(file, &info, sizeof(MAIN_FILE_INFO), &OutSize, 0) == 0)
	{
		CloseHandle(file);

		std::cout << "ERROR: Couldn't create the file." << std::endl;

		system("pause");

		return 2;
	}

	CloseHandle(file);

	std::cout << "SUCCESS: File created." << std::endl;

	system("pause");

	return 0;
}