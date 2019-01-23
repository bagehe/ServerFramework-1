#pragma once

#include "ObjAllocator.hpp"
#include "CacheTime.hpp"
#include "CommDefine.h"
#include "RegAuthPublic.hpp"

class CSessionObj : public CCacheTime
{
private:
    TNetHead_V2 m_stNetHead; // ���Կͻ��˵�socket������Ϣ��ע�����е���Ч�ֶζ��������ֽ����
    unsigned short m_unValue; // �������ݣ���NetHead�е�sockfdһ���ʶ���session
    bool m_bActive; // ��session����Ƿ���active״̬���������ʱ��Ϊactive״̬���ͷŽ��ʱ��Ϊ��active״̬

public:
    CSessionObj();
    virtual ~CSessionObj();

public:
    int Create(TNetHead_V2& stNetHead, const unsigned short unValue);
    void SetActive(bool bActive);

public:
    TNetHead_V2* GetNetHead();
    bool IsActive();
    unsigned int GetSessionFD() const;
    unsigned short GetValue() const;
    void GetSockCreatedTime(char* pszTime, unsigned int uiBufLen);
    void GetMsgCreatedTime(char* pszTime, unsigned int uiBufLen);
};
