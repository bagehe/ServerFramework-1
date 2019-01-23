#pragma once

#include <vector>
#include <set>
#include <string>

//������Ϣ
struct OrderData
{
	std::string strOrder;	//������
	int iTime;				//������ɵ�ʱ��
	unsigned uiUin;			//���uin
	int iRechargeID;		//��ֵ���

	OrderData()
	{
		Reset();
	}

	void Reset()
	{
		strOrder.clear();
		iTime = 0;
		uiUin = 0;
		iRechargeID = 0;
	}
};

class COrderManager
{
public:

	static COrderManager* Instance();
	~COrderManager();

	//��ʼ��
	void Init();

	//���Ӷ���
	void AddOrder(const OrderData& stOrderData, bool bAddAll);

	//ɾ��������
	void DeleteOrderID(const std::string& strOrder);

	//�������Ƿ����
	bool IsOrderIDExist(const std::string& strOrder);

	//��ʱ��
	void OnTick(int iTimeNow);

private:
	COrderManager();

	//���浽�ļ�
	void SaveOrderInfo();

	//���ļ��м���
	void LoadOrderInfo();

	//����
	void Reset();

private:

	//�����ż���
	std::set<std::string> m_setOrders;

	//�������б�ʱ��˳������
	std::vector<OrderData> m_vOrderDatas;
};
