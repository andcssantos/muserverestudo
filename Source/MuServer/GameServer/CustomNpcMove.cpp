#include "stdafx.h"
#include "CustomNpcMove.h"
#include "Map.h"
#include "ReadScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"

CCustomNpcMove gCustomNpcMove;

CCustomNpcMove::CCustomNpcMove()
{
	this->m_CustomNpcMove.clear();
}

CCustomNpcMove::~CCustomNpcMove()
{

}

void CCustomNpcMove::Load(char* path)
{
	CReadScript* lpReadScript = new CReadScript;

	if (lpReadScript == NULL)
	{
		ErrorMessageBox(READ_SCRIPT_ALLOC_ERROR, path);

		return;
	}

	if (!lpReadScript->Load(path))
	{
		ErrorMessageBox(READ_SCRIPT_FILE_ERROR, path);

		delete lpReadScript;

		return;
	}

	this->m_CustomNpcMove.clear();

	int Index = 0;

	try
	{
		eTokenResult token;

		while (true)
		{
			token = lpReadScript->GetToken();

			if (token == TOKEN_END || token == TOKEN_END_SECTION)
			{
				break;
			}

			NPC_MOVE_INFO info;

			info.MonsterClass = lpReadScript->GetNumber();

			info.Map = lpReadScript->GetAsNumber();

			info.X = lpReadScript->GetAsNumber();

			info.Y = lpReadScript->GetAsNumber();

			info.MoveMap = lpReadScript->GetAsNumber();

			info.MoveX = lpReadScript->GetAsNumber();

			info.MoveY = lpReadScript->GetAsNumber();

			info.MinLevel = lpReadScript->GetAsNumber();

			info.MaxLevel = lpReadScript->GetAsNumber();

			info.MinReset = lpReadScript->GetAsNumber();

			info.MaxReset = lpReadScript->GetAsNumber();

			info.AccountLevel = lpReadScript->GetAsNumber();

			info.PkMove = lpReadScript->GetAsNumber();

			this->m_CustomNpcMove.insert(std::pair<int, NPC_MOVE_INFO>(Index, info));

			Index++;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadScript->GetError());
	}

	delete lpReadScript;
}

bool CCustomNpcMove::GetNpcMove(LPOBJ lpObj, int MonsterClass, int Map, int X, int Y)
{
	for (std::map<int, NPC_MOVE_INFO>::iterator it = this->m_CustomNpcMove.begin(); it != this->m_CustomNpcMove.end(); it++)
	{
		if (it->second.MonsterClass == MonsterClass && it->second.Map == Map && it->second.X == X && it->second.Y == Y)
		{
			if (it->second.MinLevel != -1 && lpObj->Level < it->second.MinLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(59, lpObj->Lang), it->second.MinLevel);

				return 1;
			}

			if (it->second.MaxLevel != -1 && lpObj->Level > it->second.MaxLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(60, lpObj->Lang), it->second.MaxLevel);

				return 1;
			}

			if (it->second.MinReset != -1 && lpObj->Reset < it->second.MinReset)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(61, lpObj->Lang), it->second.MinReset);

				return 1;
			}

			if (it->second.MaxReset != -1 && lpObj->Reset > it->second.MaxReset)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(62, lpObj->Lang), it->second.MaxReset);

				return 1;
			}

			if (lpObj->AccountLevel < it->second.AccountLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(57, lpObj->Lang));

				return 1;
			}

			if (it->second.PkMove == 0 && lpObj->PKLevel >= PKLVL_OUTLAW)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(66, lpObj->Lang));

				return 1;
			}

			if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(65, lpObj->Lang));

				return 1;
			}

			if (it->second.MoveMap == MAP_ATLANS && (lpObj->Inventory[8].m_Index == GET_ITEM(13, 2) || lpObj->Inventory[8].m_Index == GET_ITEM(13, 3))) // Uniria, Dinorant
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(67, lpObj->Lang));

				return 1;
			}

			if (it->second.MoveMap == MAP_ICARUS && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3))) // Dinorant
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(68, lpObj->Lang));

				return 1;
			}

			if (it->second.MoveMap == MAP_ICARUS && lpObj->Inventory[8].m_Index == GET_ITEM(13, 2)) // Uniria
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(67, lpObj->Lang));

				return 1;
			}

			gObjTeleport(lpObj->Index, it->second.MoveMap, it->second.MoveX, it->second.MoveY);

			return 1;
		}
	}

	return 0;
}