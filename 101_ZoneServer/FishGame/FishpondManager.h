#pragma once

#include <map>

#include "FishpondObj.h"

class CFishpondManager
{
public:
	static CFishpondManager* Instance();
	~CFishpondManager();

public:

	//�������
	CFishpondObj* CreateFishpond(int iFishRoomID);

	//�������
	void DelFishpond(CFishpondObj& stFishpondObj);

	//���¼����������
	void ReloadFishConfig();

private:
	CFishpondManager();

	//��ȡ�������
	CFishpondObj* NewFromFishpondList(const FishRoomConfig& stConfig);

	//������ص���������
	void AddToFishpondList(const FishRoomConfig& stConfig, CFishpondObj& stFishpondObj);

private:
	
	//�����������
	std::map<int, CFishpondObj*> m_mpFreeHead;
};
