
#include <fstream>

#include "json/json.h"
#include "LogAdapter.hpp"

#include "OrderManager.h"

using namespace ServerLib;

const static char* RECHARGE_ORDER_INFO_FILE = "../conf/RechargeOrder.dat";

COrderManager* COrderManager::Instance()
{
	static COrderManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new COrderManager;
	}

	return pInstance;
}

COrderManager::~COrderManager()
{

}


COrderManager::COrderManager()
{
	Reset();
}

//��ʼ��
void COrderManager::Init()
{
	Reset();

	//���ض�����ϸ��Ϣ
	LoadOrderInfo();

	return;
}

//���Ӷ���
void COrderManager::AddOrder(const OrderData& stOrderData, bool bAddAll)
{
	if (bAddAll)
	{
		//������ϸ��Ϣ
		m_vOrderDatas.push_back(stOrderData);

		//���浽�ļ�
		SaveOrderInfo();
	}
	else
	{
		//ֻ���Ӷ�����
		m_setOrders.insert(stOrderData.strOrder);
	}

	return;
}

//ɾ��������
void COrderManager::DeleteOrderID(const std::string& strOrder)
{
	//ֻɾ��������
	m_setOrders.erase(strOrder);

	return;
}

//�������Ƿ����
bool COrderManager::IsOrderIDExist(const std::string& strOrder)
{
	return m_setOrders.count(strOrder);
}

//��ʱ��
void COrderManager::OnTick(int iTimeNow)
{
	unsigned i = 0;
	for (; i < m_vOrderDatas.size(); ++i)
	{
		//��ౣ��3Сʱ�ļ�¼
		if (m_vOrderDatas[i].iTime >= (iTimeNow - 3 * 60 * 60))
		{
			break;
		}
	}

	if (i > 0)
	{
		//��ɾ���ļ�¼
		m_vOrderDatas.erase(m_vOrderDatas.begin(), m_vOrderDatas.begin()+i);

		//���浽�ļ�
		SaveOrderInfo();
	}

	return;
}

//���浽�ļ�
void COrderManager::SaveOrderInfo()
{
	//ֻ���涩����ϸ��Ϣ

	//���涩����Ϣ���ļ�
	std::ofstream os;
	os.open(RECHARGE_ORDER_INFO_FILE, std::ios::binary);
	if (os.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open recharge order file %s\n", RECHARGE_ORDER_INFO_FILE);
		return;
	}

	//������ݳ�json��
	Json::Value jValue;

	Json::Value jOneOrder;
	for (unsigned i = 0; i < m_vOrderDatas.size(); ++i)
	{
		jOneOrder.clear();

		jOneOrder["order"] = m_vOrderDatas[i].strOrder;
		jOneOrder["time"] = m_vOrderDatas[i].iTime;
		jOneOrder["uin"] = m_vOrderDatas[i].uiUin;
		jOneOrder["rechargeid"] = m_vOrderDatas[i].iRechargeID;

		jValue.append(jOneOrder);
	}

	os << jValue.toStyledString();
	os.close();

	return;

	
}

//���ļ��м���
void COrderManager::LoadOrderInfo()
{
	//���ض�����ϸ��Ϣ
	//���ļ��ж�ȡRank��Ϣ
	std::ifstream is;
	is.open(RECHARGE_ORDER_INFO_FILE, std::ios::binary);
	if (is.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open recharge order file %s\n", RECHARGE_ORDER_INFO_FILE);
		return;
	}

	Json::Reader jReader;
	Json::Value jValue;
	if (!jReader.parse(is, jValue))
	{
		//����json�ļ�ʧ��
		is.close();
		LOGERROR("Failed to parse rank info file %s\n", RECHARGE_ORDER_INFO_FILE);
		return;
	}

	m_vOrderDatas.clear();
	m_setOrders.clear();

	OrderData stOneOrder;
	for (unsigned i = 0; i < jValue.size(); ++i)
	{
		stOneOrder.Reset();

		stOneOrder.strOrder = jValue[i]["order"].asString();
		stOneOrder.iTime = jValue[i]["time"].asInt();
		stOneOrder.uiUin = jValue[i]["uin"].asUInt();
		stOneOrder.iRechargeID = jValue[i]["rechargeid"].asInt();

		m_setOrders.insert(stOneOrder.strOrder);
		m_vOrderDatas.push_back(stOneOrder);
	}

	is.close();

	return;
}

//����
void COrderManager::Reset()
{
	m_setOrders.clear();
	m_vOrderDatas.clear();
}
