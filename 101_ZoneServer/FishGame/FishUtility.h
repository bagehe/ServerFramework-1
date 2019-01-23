#pragma once

//���㸨��������
class CFishpondObj;
class CFishUtility
{
public:
	//��ȡ����ID
	static unsigned GetTableUniqID();

	//��ȡ��ΨһID
	static unsigned GetFishUniqID();

	//��ȡ�ӵ�ΨһID
	static unsigned GetBulletUniqID();

	//��ȡ���ΨһID
	static unsigned GetReturnUniqID();

	//��ȡ����ΨһID
	static unsigned GetCycleUniqID();

	//��ȡ��ض���
	static CFishpondObj* GetFishpondByID(unsigned uTableID);

	//�ж�ʱ���Ƿ�ͬһ��
	static bool IsSameDay(long lTime1, long lTime2);

private:

	//����ΨһID
	static unsigned uTableUniqID;

	//����ΨһID
	static unsigned uFishUniqID;

	//�ӵ�ΨһID
	static unsigned uBulletUniqID;

	//���ΨһID
	static unsigned uReturnID;

	//����ΨһID
	static unsigned uCycleUniqID;
};