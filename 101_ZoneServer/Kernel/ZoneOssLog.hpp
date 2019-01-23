#ifndef __ZONE_OSS_LOG_HPP__
#define __ZONE_OSS_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "CommDefine.h"
#include "Kernel/ModuleHelper.hpp"

using namespace ServerLib;

class CGameRoleObj;

//OSS��Ӫϵͳ��־������

//�����Ӫ��־������
enum TOssLogType
{
    OSS_LOG_TYPE_INVALID			= 0,				//�Ƿ�����־��������
    OSS_LOG_TYPE_LOGIN				= 10001,			//��ҵ�¼�ʺ�
    OSS_LOG_TYPE_LOGOUT				= 10002,			//��ҵǳ��ʺ�
	OSS_LOG_TYPE_EXPLINE			= 10003,			//���������
	OSS_LOG_TYPE_HITFISH			= 10004,			//���������
	OSS_LOG_TYPE_CATCHFISH			= 10005,			//��Ҵ���
	OSS_LOG_TYPE_CATCHWARHEADFISH	= 10006,			//��Ҵ򱬵�ͷ��
	OSS_LOG_TYPE_RECHARGE			= 10007,			//��ҳ�ֵ��¼
	OSS_LOG_TYPE_VIPLOTTERY			= 10008,			//���VIP�齱 ��������
	OSS_LOG_TYPE_PRESENT			= 10009,			//�������
	OSS_LOG_TYPE_USEWARHEAD			= 10010,			//���ʹ�õ�ͷ
	OSS_LOG_TYPE_POINTRACE			= 10011,			//�����������������
	OSS_LOG_TYPE_RECHARGELOTTERY	= 10012,			//��ֵ�齱
	OSS_LOG_TYPE_EXCHANGE			= 10013,			//�һ���־
	OSS_LOG_TYPE_USEITEM			= 10014,			//����ʹ����־
	OSS_LOG_TYPE_QUEST				= 10015,			//������־
	OSS_LOG_TYPE_LOGINREWARD		= 10016,			//��¼������־
	OSS_LOG_TYPE_LASVEGAS			= 10017,			//��˹ά��˹���ӮǮ
	OSS_LOG_TYPE_LIVENESS			= 10018,			//��Ծ�ȱ�����־
	OSS_LOG_TYPE_MAIL				= 10019,			//������־
	OSS_LOG_TYPE_ALMS				= 10020,			//�ȼͽ���־
	OSS_LOG_TYPE_USESKILL			= 10021,			//����ʹ����־
	OSS_LOG_TYPE_VIPREWARD			= 10022,			//VIP���������־
	OSS_LOG_TYPE_SHOOTBULET			= 10023,			//��ҷ����ӵ�
	OSS_LOG_TYPE_GETREDPACKET		= 10024,			//��һ�ú��
	OSS_LOG_TYPE_CYCLEPROFIT		= 10025,			//������ڽ���
	OSS_LOG_TYPE_SERVERCYCLEPROFIT	= 10026,			//����������ӯ��������־
	OSS_LOG_TYPE_PLAYERBET			= 10027,			//�����˹ά��˹ת����ע��־
	OSS_LOG_TYPE_FISHCOSTBYTYPE		= 10028,			//�������ӵ�����
	OSS_LOG_TYPE_FINNEWGUIDE		= 10029,			//��������������
	OSS_LOG_TYPE_GETREWARDFROMMAIL	= 10030,			//��Ҵ��ʼ���ȡ����
	OSS_LOG_TYPE_EXCHANGEBILL		= 10031,			//���ʹ�û��ѵ��߶һ�����
	OSS_LOG_TYPE_ENTITYEXCHANGE		= 10032,			//���ʵ��һ�
};

class CZoneOssLog
{
public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Zone��־��ӡ�ӿ�

    //��¼��¼�����־
    static void TraceLogin(CGameRoleObj& stRoleObj);
    static void TraceLogout(CGameRoleObj& stRoleObj);

	//�����������־
	static void TraceExpLine(CGameRoleObj& stRoleObj, long8 iWinNum, int iExpLine, int iShootNum, long8 lCostNum);

	//��������������־
	static void TraceHitFish(unsigned uin, const char* szChannelID, int iGunID, int iFishID, unsigned uFishUniqueID, bool bIsForm,
		long8 lTotalReturnSilver, int iX, int iRedPacketPercent, int iServerX, int iUserX, int iExpLineX, int iReturnType);

	//��Ҵ�����־�����˲��㣩
	static void TraceCatchFish(unsigned uin, const char* szChannelID, const char* szNickName, int iGunMultiple, int iFishType, int iResType, int iRoomID,
		long8 lOldResNum, long8 lNewResNum);

	//��Ҵ򱬵�ͷ�㣨���˲��㣩
	static void TraceCatchWarHeadFish(unsigned uin, const char* szChannelID, const char* szNickName, int iGunMultiple, int iFishType, int iItemType, int iItemID, int iRoomID,
		long8 lOldItemNum, long8 lNewItemNum);

	//��ҳ�ֵ��־
	static void TraceRecharge(unsigned uin, const char* szChannelID, const char* szNickName, int iRechargeOption, int iAddCoin, long8 lOldNum, long8 lNewNum, int iTotalOnlineTime, int iVipLv);

	//���VIP�齱������������־
	static void TraceVipLottery(unsigned uin, const char* szChannelID, const char* szNickName, int iCostNum, int iRewardResType, int iRewardID, long8 lOldNum, long8 lNewNum);

	//���������־
	static void TracePresent(unsigned uinGive, unsigned uinRecepte, int iItemID, int iItemNum);

	//��ҵ�ͷʹ����־
	static void TraceUseWarHead(unsigned uin, const char* szChannelID, const char* szNickName, int iWarHeadID, int iAddCoinNum, int iRoomID, long8 lOldWarHeadNum,
		long8 lNewWarHeadNum, long8 lOldCoinNum, long8 lNewCoinNum);

	//��Ҳ������������־
	static void TracePointRace(unsigned uin, const char* szNickName, long8 lBounsPoints, int iRewardItemType, int iRewardID, long8 iRewardItemNum);

	//��������齱��־
	static void TraceLimitLottery(unsigned uin, const char* szChannelID, const char* szNickName, int iLimitLotteryType, int iLimitLotteryID, int iRewardType, int iRewardID, long8 lOldNum, long8 lNewNum);

	//��Ҷһ���־
	static void TraceExchange(unsigned uin, const char* szChannelID, const char* szNickName, int iExchangeOption, long8 lCostTicketNum, long8 lOldNum, long8 lNewNum, int iRewardType, int iRewardID, long8 lOldRewardNnum, long8 lNewRewardNum);

	//��ҵ���ʹ����־
	static void TraceUseItem(unsigned uin, const char* szChannelID, const char* szNickName, int iItemID, long8 lOldNum, long8 lNewNum, int iRewardID, long8 lOldRewardNum, long8 lNewRewardNum);

	//������������־
	static void TraceQuest(unsigned uin, const char* szChannelID, const char* szNickName, int iQuestID, int iQuestType, int iRewardType, int iRewardID, long8 lOldNum, long8 lNewNum);

	//��������������־
	static void TraceLoginReward(unsigned uin, const char* szChannelID, const char* szNickName, int iLoginRewardID, int iday, long8 lOldNum, long8 lNewNum);

	//�����˹ά��˹ת�� ӮǮ ��־
	static void TraceLasvegas(unsigned uin, const char* szChannelID, const char* szNickName, int iBetType, int iBetNum, int iLotteryNum, int iAddCoin, long8 lOldNum, long8 lNewNum);

	//�����˹ά��˹��ע��־
	static void TraceLasvegasBet(unsigned uin, const char* szChannelID, const char* szNickName, int iBetType, int iBetNum);

	//��һ�Ծ�ȱ�����־
	static void TraceLiveness(unsigned uin, const char* szChannelID, const char* szNickName, int iBoxID, int iRewardType, int iRewardID, long8 lOldNum, long8 lNewNum);

	//���������־
	static void TraceMail(unsigned uin, const char* szChannelID, const char* szNickName, unsigned uUinqID, const char* szTitle, int iRewardType, int iRewardID, int iRewardNum, int iMailID, int iSendTime);

	//��Ҿȼý���ȡ��־
	static void TraceAlms(unsigned uin, const char* szChannelID, const char* szNickName, int iReciveTimes, int iAddCoin, long8 lOldNum, long8 lNewNum);

	//��Ҽ���ʹ����־
	static void TraceUseSkill(unsigned uin, const char* szChannelID, const char* szNickName, int iSkillID, int iRoomID, long8 lOldNum, long8 lNewNum);

	//���VIP��¼��ȡ��־
	static void TraceVipReward(unsigned uin, const char* szChannelID, const char* szNickName, int iVipLv, long8 lOldNum, long8 lNewNum);

	//��ҷ����ӵ�
	static void TraceShootBullet(unsigned uin, const char* szChannelID, const char* szNickName, int iConsume, int iRoomID);

	//��һ�ú��
	static void TraceGetRedPacket(unsigned uin, const char* szNickName, int iRedPacketType);

	//������ڽ�����־
	static void TraceCycleProfit(unsigned uin, const char* szChannelID, const char* szNickName, int iLossNum, long8 lNewUsedNum, long8 lCoinNum, long8 lTicketNum);

	//���������ڽ�����־
	static void TraceServerCycleProfit(int iAlgorithmType, long8 lRewardSilver, long8 lCostSilver, long8 lServerWinNum,
		long8 lTicketFishDropNum, int iPump, long8 lPlayingNum, int iAverageGunMultiple, long8 lTotalReturnSilver, long8 lTicketReturnNum,
		long8 lNextReturnTime, int iReturnPlayerNum, long8 lUsedReturnSilver);

	//��¼��ͷ����ˮ
	static void TraceShootCostByFishType(unsigned uin, const char* szChannelID, const char* szNickName, int iRoomID, int iGunMultiple, int iFishType);

	//��¼�����������
	static void TraceFinNewGuide(unsigned uin, const char* szChannelID, const char* szNickName, int iGuideID);

	//��¼��Ҵ��ʼ���ȡ����
	static void TraceGetRewardFromMail(unsigned uin, const char* szChannelID, const char* szNickName, int iReward, int iRewardNum, int iOldNum, int iNewNum);

	//��¼���ʵ��һ�
	static void TraceEntityExchange(unsigned uin, int iExchangeID, const std::string& strOrderID, int iTime, const std::string& strName, const std::string& strPhone, 
		const std::string& strMailNum, const std::string& strAddress, const std::string& strRemarks, const std::string& strQQNum);

	//��¼���ʹ�û��ѵ��߶һ�����
	static void TraceExchangeBill(unsigned uin, const char* szChannelID, const char* szNickName, int iItemID, int iItemNum, int iExchangeNum, const char* szPhoneNum, int iOldNum, int iNewNum);

	//��¼�ϱ���־
	static void ReportUserLogin(unsigned uin, const char* szAccount, const char* szDeviceID, const char* szChannelID, bool bIsNew, const char* szClientIP);

	//��ֵ�ϱ���־
	static void ReportUserPay(unsigned uin, const char* szAccount, const char* szDeviceID, const char* szChannelID, const char* szOrderID, int iRMB, int iItemNum, const char* szClientIP);

	//���ڽ����ϱ���־
	static void ReportTallyData(CGameRoleObj& stRoleObj);
};

#endif
