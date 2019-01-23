#pragma once

#include <string>
#include <vector>

#include "Kernel/GameRole.hpp"

//�������Ϣ
struct HorseLampData
{
	int iLampID;	//�����ID
	int iEndTime;	//����ʱ�䣬Ϊ0����Ч
	int iInterval;	//����ʱ������Ϊ0����Ч
	int iTimes;		//���Ŵ�����Ϊ0���ù�
	std::vector<std::string> vParams;	//����Ʋ���

	HorseLampData()
	{
		Reset();
	}

	void Reset()
	{
		iLampID = 0;
		iEndTime = 0;
		iInterval = 0;
		iTimes = 0;
		vParams.clear();
	}
};

//���칤����
class CChatUtility
{
public:
	//���������
	static void SendHorseLamp(std::vector<HorseLampData> vData);
};
