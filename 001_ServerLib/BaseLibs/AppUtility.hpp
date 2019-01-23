#ifndef __APP_UTILITY_HPP__
#define __APP_UTILITY_HPP__

#include "SignalUtility.hpp"

namespace ServerLib
{
	const int APPCMD_NOTHING_TODO = 0;          //ɶҲ����
	const int APPCMD_STOP_SERVICE = 1;          //ֹͣ����
	const int APPCMD_RELOAD_CONFIG = 2;          //�ض���������
	const int APPCMD_QUIT_SERVICE = 3;          //��������ѭ��

	class CAppUtility
	{
	public:
		// ��������������, ��ɵĹ�����:
		// 1. ע��USR������, ����ֹͣ/ˢ�·�����
		// 2. ֧��ԭ�еĲ�����TCM����, ����Ƿ�ָ�ģʽ����, ����TBUS��ַ
		// 3. ԭ�еĲ�����-r, -w, -insָ���ָ�ģʽ��TBUS��ַ
		// 4. TAppģʽ�Ľ���, ʹ��--resume��--idָ���ָ�ģʽ��TBUS��ַ
		static void AppLaunch(int argc, char** argv,
			Function_SignalHandler pSigHandler,
			bool& rbResume,
			int& riWorldID,
			int& riInstanceID,
			int* piZoneID = NULL,
			char* pszHostIP = NULL,
			int* piHostPort = NULL,
			bool bEnableQuitSig = false);

	public:
		// ��ʼ����־����
		// pszConfigFile: �����ļ�. NULL��ʹ��Ĭ������
		// pszLogName: ��־�ļ���
		static void LoadLogConfig(const char* pszConfigFile, const char* pszLogName);

	public:
		// ע��ֹͣ�����������źŴ���
		static void RegisterSignalHandler(Function_SignalHandler pSigHandler, bool bEnableQuitSig);

	private:
		// ��ӡ�汾
		static void ShowVersion();

		// ��������
		static void ShowUsage(const char* pszName);

		//����ļ�������ֹ�ظ�����
		static void CheckLock(const char* pszLockFile);

		//��ʼ��Ϊ�ػ����̵ĺ���
		static void DaemonLaunch(void);

		//����������˳���pid�ļ�����ɾ��������Ϊ�쳣��ֹ
		//����pid�ɱ��ű������رս��̡����¶�ȡ�����ļ�
		static void WritePidFile();

		// ���PID�ļ�
		static void CleanPidFile();

		// ��ȡPID�ļ�
		static int ReadPidFile();

	private:
		// ֧���ļ���������
		static int ReadConfigFile(const char* pszFilename, bool* pbDaemonLaunch, bool* pbResume, int *piWorldID, int *piZoneID, int *piInstance);
	};
}

#endif