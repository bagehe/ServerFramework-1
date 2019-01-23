#include "CommDefine.h"
#include "Kernel/HandlerFactory.hpp"
#include "HandlerList.hpp"
#include "Kernel/ModuleHelper.hpp"

int CHandlerList::RegisterAllHandler()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DB��ȡ����
    CHandlerFactory::RegisterHandler(MSGID_WORLD_FETCHROLE_RESPONSE, &m_stFetchRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_UPDATEROLE_RESPONSE,&m_stUpdateRoleInfoHandler);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ��¼�ǳ�
    CHandlerFactory::RegisterHandler(MSGID_ZONE_LOGINSERVER_REQUEST, &m_stLoginHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    //CHandlerFactory::RegisterHandler(MSGID_LOGINSERVER_NOTIFY, &m_stLoginHandler);
    CHandlerFactory::RegisterHandler(MSGID_LOGOUTSERVER_REQUEST, &m_stLogoutHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    //CHandlerFactory::RegisterHandler(MSGID_LOGOUTSERVER_NOTIFY, &m_stLogoutHandler);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ���˷��
    CHandlerFactory::RegisterHandler(MSGID_WORLD_KICKROLE_REQUEST, &m_stKickRoleWorldHandler);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_KICKROLE_RESPONSE, &m_stKickRoleWorldHandler);
    //CHandlerFactory::RegisterHandler(MSGID_FORBIDROLEREQUEST, &m_stKickRoleWorldHandler);

    //GM����ִ��
    CHandlerFactory::RegisterHandler(MSGID_ZONE_GAMEMASTER_REQUEST, &m_stGMCommandHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_GAMEMASTER_REQUEST, &m_stGMCommandHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_GAMEMASTER_RESPONSE, &m_stGMCommandHandler);

    //��Ʒ��ز���
    CHandlerFactory::RegisterHandler(MSGID_ZONE_REPOPERA_REQUEST, &m_stRepThingsHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_ONLINEEXCHANGE_RESPONSE, &m_stRepThingsHandler);

	//�齱��ز���
	CHandlerFactory::RegisterHandler(MSGID_ZONE_LOTTERY_REQUEST, &m_stLotteryHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_LIMITLOTTERY_REQUEST, &m_stLotteryHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_PAYLOTTERYRECORD_REQUEST, &m_stLotteryHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_ENTERLASVEGAS_REQUEST, &m_stLotteryHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_LASVEGASBET_REQUEST, &m_stLotteryHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETREWARDINFO_REQUEST, &m_stLotteryHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_LIMITLOTTERY_RESPONSE, &m_stLotteryHandler);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_PAYLOTTERYRECORD_RESPONSE, &m_stLotteryHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_UPDATELASVEGAS_NOTIFY, &m_stLotteryHandler);

    //�������
    CHandlerFactory::RegisterHandler(MSGID_ZONE_CHAT_REQUEST, &m_stChatHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    CHandlerFactory::RegisterHandler(MSGID_WORLD_CHAT_NOTIFY, &m_stChatHandler, EKMT_SERVER, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_HORSELAMP_NOTIFY, &m_stChatHandler, EKMT_SERVER, EKMF_CHECKNONE);

	//�������
	CHandlerFactory::RegisterHandler(MSGID_ZONE_DOFISH_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_EXITFISH_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_CHANGEGUN_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_SHOOTBULLET_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_HITFISH_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_HITFORMFISH_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_USESKILL_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_CHOOSEAIMFISH_REQUEST, &m_stFishpondHandler, EKMT_CLIENT, EKMF_CHECKNONE);

	//�������
	CHandlerFactory::RegisterHandler(MSGID_ZONE_FINQUEST_REQUEST, &m_stQuestHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETLIVNESS_REQUEST, &m_stQuestHandler, EKMT_CLIENT, EKMF_CHECKNONE);

	//�һ����
	CHandlerFactory::RegisterHandler(MSGID_ZONE_SETEXCHANGE_REQUEST, &m_stExchangeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_EXCHANGEITEM_REQUEST, &m_stExchangeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETLIMITNUM_REQUEST, &m_stExchangeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETEXCHANGEREC_REQUEST, &m_stExchangeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETLIMITNUM_RESPONSE, &m_stExchangeHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_ADDLIMITNUM_RESPONSE, &m_stExchangeHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_GETCARDNO_RESPONSE, &m_stExchangeHandler);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETEXCHANGEREC_RESPONSE, &m_stExchangeHandler);

	//���а����
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETRANKINFO_REQUEST, &m_stRankInfoHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_GETRANKINFO_RESPONSE, &m_stRankInfoHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_UPDATERANK_RESPONSE, &m_stRankInfoHandler);

	//�ʼ����
	CHandlerFactory::RegisterHandler(MSGID_ZONE_MAILOPERA_REQUEST, &m_stMailHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_SENDMAIL_REQUEST, &m_stMailHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_SENDMAIL_RESPONSE, &m_stMailHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_SYSTEMMAILID_NOTIFY, &m_stMailHandler);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_GETSYSTEMMAIL_RESPONSE, &m_stMailHandler);
	
	//VIP���
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETUSERALMS_REQUEST, &m_stVipPrivHandler, EKMT_CLIENT, EKMF_CHECKNONE);

	//�������
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETLOGINREWARD_REQUEST, &m_stRewardHandler, EKMT_CLIENT, EKMF_CHECKNONE);

	//��ֵ���
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETPAYRECORD_REQUEST, &m_stRechargeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETPAYGIFT_REQUEST, &m_stRechargeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETPAYORDER_REQUEST, &m_stRechargeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_GETPAYORDER_RESPONSE, &m_stRechargeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_USERRECHARGE_REQUEST, &m_stRechargeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_USERRECHARGE_RESPONSE, &m_stRechargeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_WORLD_USERRECHARGE_REQUEST, &m_stRechargeHandler);

	//�����������
	CHandlerFactory::RegisterHandler(MSGID_ZONE_FINGUIDE_REQUEST, &m_stNewGuideHandler, EKMT_CLIENT, EKMF_CHECKNONE);
	CHandlerFactory::RegisterHandler(MSGID_ZONE_UPDATENAME_REQUEST, &m_stNewGuideHandler, EKMT_CLIENT, EKMF_CHECKNONE);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ʱ��ͬ��
    //CHandlerFactory::RegisterHandler(MSGID_SYNCTIME_REQUEST, &m_stSyncTimeHandler, EKMT_CLIENT, EKMF_CHECKNONE);
    //CHandlerFactory::RegisterHandler(MSGID_SYNCTIME_CLIENT_REQUEST, &m_stSyncTimeHandler, EKMT_CLIENT, EKMF_CHECKNONE);

    return 0;
}
