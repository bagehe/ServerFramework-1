#pragma once

#include "RegAuthPublic.hpp"
#include "SessionObj.hpp"
#include "SingletonTemplate.hpp"
#include "CommDefine.h"

static const int MAX_CODEQUEUE_NUM = 2;

class CSessionManager
{
public:
	static CSessionManager* Instance();
	~CSessionManager();

    int Initialize(bool bResume);
    int CheckSession(const TNetHead_V2& stNetHead);
    CSessionObj* CreateSession(TNetHead_V2& stNetHead);
    CSessionObj* GetSession(const unsigned int uiSessionFD, const unsigned short unValue = 0);
    int DeleteSession(const unsigned int uiSessionFD);

private:
	CSessionManager();
    unsigned short GenerateValue();
	CSessionObj* GetSessionByFd(unsigned int uiSessionFD);

private:

	CSessionObj m_astSessionList[MAX_CODEQUEUE_NUM][MAX_ACCOUNT_OBJ_CACHE_NUMBER];
	unsigned short m_unSeed; // ��ʶsession�ĸ������ݵ����ӣ�����ʼֵ
	unsigned int m_uiActiveNumber; // ����active״̬�Ļ�����ĸ���
};
