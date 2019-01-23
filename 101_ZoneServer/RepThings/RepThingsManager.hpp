#ifndef __REP_THINGS_MANAGER_HPP__
#define __REP_THINGS_MANAGER_HPP__

#include <vector>

#include "CommDefine.h"
#include "GameProtocol.hpp"

struct RepItem
{
    int iItemID;
    int iItemNum;

    RepItem()
    {
        Reset();
    };

    void Reset()
    {
        iItemID = 0;
        iItemNum = 0;
    };
};

class CGameRoleObj;
class CRepThingsManager
{
public:
    CRepThingsManager();
    ~CRepThingsManager();

public:

    //��ʼ��
    int Initialize();

    void SetOwner(unsigned int uin);
    CGameRoleObj* GetOwner();

    //����������Ʒ
    int AddRepItem(int iItemID, int iItemNum, int iItemChannel = 0);

    //����ɾ����Ʒ
    int DeleteRepItem(int iItemID, int iItemNum, int iItemChannel = 0);

    //��ȡ��������Ʒ������
    int GetRepItemNum(int iItemID);

    //������Ʒ���ݿ��������
    void UpdateRepItemToDB(ITEMDBINFO& stItemInfo);
    void InitRepItemFromDB(const ITEMDBINFO& stItemInfo);

private:

	RepItem* GetRepItemInfo(int iItemID);

    //���ͱ�����Ʒ�仯��֪ͨ
    void SendRepChangeNotify(int iItemID, int iItemNum, int iItemChannel = 0);

private:

    //������uin
    unsigned int m_uiUin;

    //�����е���Ʒ��Ϣ
    std::vector<RepItem> m_vstRepItems; 

    //��ұ�����Ʒ�����仯��֪ͨ
    static GameProtocolMsg m_stRepChangeNotify;
};

#endif
