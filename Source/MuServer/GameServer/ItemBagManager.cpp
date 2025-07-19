#include "stdafx.h"
#include "ItemBagManager.h"
#include "ReadScript.h"
#include "Path.h"
#include "Util.h"
#include "ItemManager.h"

CItemBagManager gItemBagManager;

CItemBagManager::CItemBagManager()
{
	this->m_ItemBagManagerInfo.clear();
}

CItemBagManager::~CItemBagManager()
{

}

void CItemBagManager::Load(char* path)
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

	this->m_ItemBagManagerInfo.clear();

	int index = 0;

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

			ITEM_BAG_MANAGER_INFO info;

			info.Index = index++;

			info.ItemIndex = lpReadScript->GetNumber();

			if (info.ItemIndex != -1)
			{
				info.ItemIndex = SafeGetItem(GET_ITEM(info.ItemIndex, lpReadScript->GetAsNumber()));
			}

			info.ItemLevel = lpReadScript->GetAsNumber();

			info.MonsterClass = lpReadScript->GetAsNumber();

			info.SpecialValue = lpReadScript->GetAsNumber();

			this->m_ItemBagManagerInfo.insert(std::pair<int, ITEM_BAG_MANAGER_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadScript->GetError());
	}

	delete lpReadScript;
}

void CItemBagManager::LoadEventItemBag()
{
	std::map<int, int> LoadEventItemBag;

	char wildcard_path[MAX_PATH];

	wsprintf(wildcard_path, "%s*", gPath.GetFullPath("EventItemBag\\"));

	WIN32_FIND_DATA data;

	HANDLE file = FindFirstFile(wildcard_path, &data);

	if (file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if (isdigit(data.cFileName[0]) != 0 && isdigit(data.cFileName[1]) != 0 && isdigit(data.cFileName[2]) != 0)
			{
				if (data.cFileName[3] == ' ' && data.cFileName[4] == '-' && data.cFileName[5] == ' ')
				{
					std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.find(atoi(data.cFileName));

					if (it != this->m_ItemBagManagerInfo.end())
					{
						if (LoadEventItemBag.find(it->first) == LoadEventItemBag.end())
						{
							char path[MAX_PATH];

							wsprintf(path, "EventItemBag\\%s", data.cFileName);

							it->second.ItemBag.Load(gPath.GetFullPath(path));

							LoadEventItemBag.insert(std::pair<int, int>(it->first, 1));
						}
					}
				}
			}
		}
	}
	while (FindNextFile(file, &data) != 0);
}

bool CItemBagManager::GetItemByItemIndex(int ItemIndex, int ItemLevel, LPOBJ lpObj, CItem* lpItem)
{
	for (std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if (it->second.ItemIndex != -1 && it->second.ItemIndex == ItemIndex && (it->second.ItemLevel == -1 || it->second.ItemLevel == ItemLevel))
		{
			return it->second.ItemBag.GetItem(lpObj, lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::GetItemByMonsterClass(int MonsterClass, LPOBJ lpObj, CItem* lpItem)
{
	for (std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if (it->second.MonsterClass != -1 && it->second.MonsterClass == MonsterClass)
		{
			return it->second.ItemBag.GetItem(lpObj, lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::GetItemBySpecialValue(int SpecialValue, LPOBJ lpObj, CItem* lpItem)
{
	for (std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if (it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{
			return it->second.ItemBag.GetItem(lpObj, lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemByItemIndex(int ItemIndex, int ItemLevel, LPOBJ lpObj, int map, int x, int y)
{
	for (std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if (it->second.ItemIndex != -1 && it->second.ItemIndex == ItemIndex && (it->second.ItemLevel == -1 || it->second.ItemLevel == ItemLevel))
		{
			return it->second.ItemBag.DropItem(lpObj, map, x, y);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemByMonsterClass(int MonsterClass, LPOBJ lpObj, int map, int x, int y)
{
	for (std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if (it->second.MonsterClass != -1 && it->second.MonsterClass == MonsterClass)
		{
			return it->second.ItemBag.DropItem(lpObj, map, x, y);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemBySpecialValue(int SpecialValue, LPOBJ lpObj, int map, int x, int y)
{
	for (std::map<int, ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if (it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{
			return it->second.ItemBag.DropItem(lpObj, map, x, y);
		}
	}

	return 0;
}