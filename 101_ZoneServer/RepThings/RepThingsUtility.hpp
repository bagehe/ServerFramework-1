#pragma once

#include "CommDefine.h"

//��������������
class CGameRoleObj;
class CRepThingsUtility
{
public:

    //��ɾ��Ʒ�Ľӿ�,���iAddNumС��0��ʾɾ��
    static int AddItemNum(CGameRoleObj& stRoleObj, int iItemID, int iAddNum, int iItemChannel);

	//������
	static int OpenBoxGift(CGameRoleObj& stRoleObj, int iBoxID, int iBoxNum = 1, bool bDeleteItem = false);

	//����ʧ�ܣ��������߸����
	static void OnSendGiftFailed(unsigned uiUin, unsigned uiToUin, int iItemID, int iItemNum, int iErrorNum);
};
