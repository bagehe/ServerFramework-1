#pragma once

#include "GameProtocol.hpp"
#include "Kernel/Handler.hpp"
#include "CommDefine.h"
#include "ErrorNumDef.hpp"

class CGameRoleObj;
class CFishpondHandler : public IHandler
{
public:
	virtual ~CFishpondHandler(void);
	CFishpondHandler();
public:
	int OnClientMsg();

private:

	//���������Ϣ����
	int OnRequestDoFish();

	//�˳������Ϣ����
	int OnRequestExitFish();

	//����л���̨
	int OnRequestChangeGun();

	//��ҷ����ӵ�
	int OnRequestShootBullet();

	//���������
	int OnRequestHitFish();

	//�������������
	int OnRequestHitFormFish();

	//���ʹ�ü���
	int OnRequestUseSkill();

	//���ѡ����׼��
	int OnRequestChooseAimFish();
};