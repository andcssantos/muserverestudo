#pragma once

struct CUSTOM_ITEM_INFO
{
	short ItemIndex;
	BYTE SkillNumber;
	char ModelName[32];
};

class CCustomItem
{
public:

	CCustomItem();

	virtual ~CCustomItem();

	void Init(CUSTOM_ITEM_INFO* info);

	void SetInfo(CUSTOM_ITEM_INFO info);

	CUSTOM_ITEM_INFO* GetInfoByIndex(int ItemIndex);

	BYTE GetCustomItemSkill(int ItemIndex);

private:

	void Init();

public:

	CUSTOM_ITEM_INFO m_CustomItemInfo[MAX_ITEM];
};

extern CCustomItem gCustomItem;