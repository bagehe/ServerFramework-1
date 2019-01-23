
#include <stddef.h>

#include "StringUtility.hpp"
#include "AppDef.hpp"
#include "ThreadLogManager.hpp"
#include "ConfigManager.hpp"

#include "MofangLogProxy.hpp"

using namespace ServerLib;

//дħ����־
int CMofangLogProxy::WriteLog(DBClientWrapper* pDataBase, char* pQueryBuff, int iThreadIndex, const std::string& strLogData)
{
	if (!pDataBase || !pQueryBuff)
	{
		TRACE_THREAD(iThreadIndex, "Failed to write mofang log, invalid param!\n");
		return -1;
	}

	//ƴװ����
	int iLength = 0;
	int iRet = GenerateQueryString(pQueryBuff, iLength, iThreadIndex, strLogData);
	if (iRet)
	{
		TRACE_THREAD(iThreadIndex, "Failed to generate query string, ret %d\n", iRet);
		return iRet;
	}

	//д�뵽DB
	iRet = WriteMofangLog(pDataBase, pQueryBuff, iLength, iThreadIndex);
	if (iRet)
	{
		TRACE_THREAD(iThreadIndex, "Failed to write log to mofang, ret %d\n", iRet);
		return iRet;
	}

	return 0;
}

//�������ݿ������
int CMofangLogProxy::GenerateQueryString(char* pQueryBuff, int& iLength, int iThreadIndex, const std::string& strLogData)
{
	//��־��ʽ����: ���ʹ���Ϊ����
	//1.��¼��־��	REPORT_LOG_MOFANGLOGIN|areaid,gameid,numid,username,deviceid,channelid,is_new,clientip, clienttype(2)
	//2.��ֵ��־:	REPORT_LOG_MOFANGPAY|areaid,gameid,numid,username,deviceid,channelid,orderid,rmb,rewardnum,ip, clienttype(2)
	//3.������־��	REPORT_LOG_TALLYINFO|numid,deviceid,channelid,coin,ticket,cash,bronze,silver,gold,bindbronze,bindsilver,bindgold,addcoin,
	//				addticket,addcash,addwin,totalwin

	int iReportLogType = REPORT_LOG_INVALID;
	char szLogData[512] = { 0 };

	sscanf(strLogData.c_str(), "%d|%s", &iReportLogType, szLogData);

	switch (iReportLogType)
	{
	case REPORT_LOG_MOFANGLOGIN:
	{
		//��¼��־
		iLength = SAFE_SPRINTF(pQueryBuff, MAX_QUERYBUFF_SIZE - 1, \
			"insert into %s(areaid,gameid,numid,username,deviceid,channelid,newplayer,ip,clienttype,logtime) "
			"values(%s,now())",
			MYSQL_MOFANGLOGIN_TABLE,
			szLogData);
	}
	break;

	case REPORT_LOG_MOFANGPAY:
	{
		//��¼��־
		iLength = SAFE_SPRINTF(pQueryBuff, MAX_QUERYBUFF_SIZE - 1, \
			"insert into %s(areaid,gameid,numid,username,deviceid,channelid,orderid,amount,virtualcurrency,ip,clienttype,logtime) "
			"values(%s,now())",
			MYSQL_MOFANGPAY_TABLE,
			szLogData);
	}
	break;

	case REPORT_LOG_TALLYINFO:
	{
		//��ҽ�����־
		iLength = SAFE_SPRINTF(pQueryBuff, MAX_QUERYBUFF_SIZE - 1, \
			"insert into %s(numid,deviceid,channelid,coin,ticket,cash,bronze,silver,gold,bindbronze,bindsilver,bindgold,addcoin,addticket,addcash,addwin,totalwin,logtime) "
			"values(%s,now())",
			MYSQL_TALLYINFO_TABLE,
			szLogData);
	}
	break;

	default:
	{
		TRACE_THREAD(iThreadIndex, "Failed to write mofang log, invalid type %d\n", iReportLogType);
		return -16;
	}
	break;
	}

	return 0;
}

//�����ݿ�д����־
int CMofangLogProxy::WriteMofangLog(DBClientWrapper* pDataBase, char* pQueryBuff, int iLength, int iThreadIndex)
{
	//�������ӵ�DB
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetLogDBConfigByIndex(iThreadIndex);
	if (!pstDBConfig)
	{
		TRACE_THREAD(iThreadIndex, "Failed to get name db config, index %d\n", iThreadIndex);
		return -21;
	}

	int iRet = pDataBase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
	if (iRet)
	{
		TRACE_THREAD(iThreadIndex, "Failed to set mysql db info, ret %d\n", iRet);
		return -22;
	}

	iRet = pDataBase->ExecuteRealQuery(pQueryBuff, iLength, false);
	if (iRet)
	{
		TRACE_THREAD(iThreadIndex, "Failed to execute sql query, ret %d\n", iRet);
		return -23;
	}

	TRACE_THREAD(iThreadIndex, "Success to write mofang log: %s\n", pQueryBuff);

	return 0;
}
