#include "stdafx.h"
#include "CharacterManager.h"
#include "ServerManager.h"
#include "Util.h"

int gServerCount = 0;

int EnableSpecialCharacters = GetPrivateProfileInt("Syntax", "EnableSpecialCharacters", 0, ".\\DataServer.ini");

void ErrorMessageBox(char* message, ...)
{
	char buff[256];

	memset(buff, 0, sizeof(buff));

	va_list arg;

	va_start(arg, message);

	vsprintf_s(buff, message, arg);

	va_end(arg);

	MessageBox(0, buff, "Error", MB_OK | MB_ICONERROR);

	ExitProcess(0);
}

void LogAdd(eLogColor color, char* text, ...)
{
	tm today;

	time_t ltime;

	time(&ltime);

	if (localtime_s(&today, &ltime) != 0)
	{
		return;
	}

	char time[32];

	if (asctime_s(time, sizeof(time), &today) != 0)
	{
		return;
	}

	char temp[1024];

	va_list arg;

	va_start(arg, text);

	vsprintf_s(temp, text, arg);

	va_end(arg);

	char log[1024];

	wsprintf(log, "%.8s %s", &time[11], temp);

	gServerDisplayer.LogAddText(color, log, strlen(log));
}

void ConsoleProtocolLog(int type, BYTE* lpMsg, int size)
{
	BYTE head, subhead;

	BYTE header = lpMsg[0];

	if (header == 0xC1 || header == 0xC3)
	{
		head = lpMsg[2];
	}
	else if (header == 0xC2 || header == 0xC4)
	{
		head = lpMsg[3];
	}

	subhead = ((header == 0xC1) ? lpMsg[3] : lpMsg[4]);

	gConsole.Output(type, "[%s] Header: 0x%02X, Head: 0x%02X, SubHead: 0x%02X, Size: %d", (type == CON_PROTO_TCP_RECV) ? "RECV" : "SEND", header, head, subhead, size);
}

bool GetCharacterSlot(char CharacterName[5][11], char* name, BYTE* slot)
{
	for (int n = 0; n < 5; n++)
	{
		if (_stricmp(CharacterName[n], name) == 0)
		{
			(*slot) = n;

			return true;
		}
	}

	return false;
}

bool CheckTextSyntax(char* text, int size)
{
	for (int n = 0; n < size; n++)
	{
		if (text[n] == 0x20 || text[n] == 0x22 || text[n] == 0x27)
		{
			return false;
		}
	}

	return true;
}

bool CheckSpecialText(char* Text)
{
	if (EnableSpecialCharacters != 0)
	{
		return true;
	}

	for (unsigned char* lpszCheck = (unsigned char*)Text; *lpszCheck; ++lpszCheck)
	{
		if (_mbclen(lpszCheck) == 1) // One byte
		{
			if (*lpszCheck < 48 || (58 <= *lpszCheck && *lpszCheck < 65) || (91 <= *lpszCheck && *lpszCheck < 97) || *lpszCheck > 122)
			{
				return false;
			}
		}
		else // Two bytes
		{
			unsigned char* lpszTrail = lpszCheck + 1;

			if (0x81 <= *lpszCheck && *lpszCheck <= 0xC8) // Korean
			{
				if ((0x41 <= *lpszTrail && *lpszTrail <= 0x5A)
				    || (0x61 <= *lpszTrail && *lpszTrail <= 0x7A)
				    || (0x81 <= *lpszTrail && *lpszTrail <= 0xFE))
				{ // Excluding transparent characters
					// Areas of special characters that are not allowed
					if (0xA1 <= *lpszCheck && *lpszCheck <= 0xAF && 0xA1 <= *lpszTrail)
					{
						return false;
					}
					else if (*lpszCheck == 0xC6 && 0x53 <= *lpszTrail && *lpszTrail <= 0xA0)
					{
						return false;
					}
					else if (0xC7 <= *lpszCheck && *lpszCheck <= 0xC8 && *lpszTrail <= 0xA0)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}

			++lpszCheck;
		}
	}

	return true;
}

int GetFreeServerIndex()
{
	int index = -1;

	int count = gServerCount;

	if (SearchFreeServerIndex(&index, 0, MAX_SERVER, 10000) != 0)
	{
		return index;
	}

	for (int n = 0; n < MAX_SERVER; n++)
	{
		if (gServerManager[count].CheckState() == false)
		{
			return count;
		}
		else
		{
			count = (((++count) >= MAX_SERVER) ? 0 : count);
		}
	}

	return -1;
}

int SearchFreeServerIndex(int* index, int MinIndex, int MaxIndex, DWORD MinTime)
{
	DWORD CurOnlineTime = 0;

	DWORD MaxOnlineTime = 0;

	for (int n = MinIndex; n < MaxIndex; n++)
	{
		if (gServerManager[n].CheckState() == false && gServerManager[n].CheckAlloc() != false)
		{
			if ((CurOnlineTime = (GetTickCount() - gServerManager[n].m_OnlineTime)) > MinTime && CurOnlineTime > MaxOnlineTime)
			{
				(*index) = n;

				MaxOnlineTime = CurOnlineTime;
			}
		}
	}

	return (((*index) == -1) ? 0 : 1);
}

WORD GetServerCodeByName(char* name)
{
	CHARACTER_INFO CharacterInfo;

	if (gCharacterManager.GetCharacterInfo(&CharacterInfo, name) == false)
	{
		return 0xFFFF;
	}
	else
	{
		return CharacterInfo.GameServerCode;
	}
}

CServerManager* FindServerByCode(WORD ServerCode)
{
	for (int n = 0; n < MAX_SERVER; n++)
	{
		if (gServerManager[n].CheckState() != false && gServerManager[n].m_ServerCode == ServerCode)
		{
			return &gServerManager[n];
		}
	}

	return 0;
}