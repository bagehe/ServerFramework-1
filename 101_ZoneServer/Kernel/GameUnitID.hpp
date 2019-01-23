#pragma once

//��Ϸ��λID
#include "ObjIdxList.hpp"
#include "ObjAllocator.hpp"

using namespace ServerLib;

class CGameUnitID : public CObj
{
public:
    CGameUnitID() {};
    virtual ~CGameUnitID() {};
    virtual int Initialize();
    virtual int Resume();

public:
    DECLARE_DYN

public:
    // �󶨶���Idx
    void BindObject(int iObjectIdx);

    // ��ȡ�󶨶����Idx
    int GetBindObjectIdx();

    // ��ȡ�󶨶��������
    TUNITINFO* GetBindUnitInfo();

private:
    // ���ö��������
    void BindUnitInfo();

private: 
    int m_iBindObjectIdx;
    TUNITINFO* m_pstUnitInfo;
};
