/**
*@file BacktraceInfo.hpp
*@author jasonxiong
*@date 2009-11-30
*@version 1.0
*@brief ��ջģ��
*
*	ͨ�����ļ������һЩ���ȡ��ջ��Ϣ
*/

#ifndef __BACK_TRACE_INFO_HPP__
#define __BACK_TRACE_INFO_HPP__

#include "SingletonTemplate.hpp"

namespace ServerLib
{

	const int MAX_BACKTRACE_SYMBOLS_NUMBER = 20; //!<����ӡ�Ķ�ջ�����
	const int MAX_BACKTRACE_STRING_LENGTH = 10240; //!<��ջ��ӡ��Ϣ����󳤶�

	class CBacktraceInfo
	{
	public:
		CBacktraceInfo();
		~CBacktraceInfo();

	public:
		/**
		*��ȡ���ж�ջ��Ϣ
		*@return ���ַ��������Ķ�ջ��Ϣ
		*/
		const char* GetAllBackTraceInfo();

		/**
		*��ȡָ���Ķ�ջ��Ϣ
		*@param[in]
		*@param[in]
		*@return 0 success
		*/
		const char* GetBackTraceInfo(int iTraceDepth);

		/**
		*���ɶ�ջ��Ϣ���ڻ�ȡǰ��Ҫ����
		*@param[in]
		*@param[in]
		*@return 0 success
		*/
		int GenBacktraceInfo();

	private:
		void* m_apBacktraceSymbols[MAX_BACKTRACE_SYMBOLS_NUMBER]; //!<��backtrace���صĶ�ջ���ŵ�ַ�������
		char** m_apszBacktraceInfo; //!<��backtrace_symbols���صĶ�ջ��Ϣ
		int m_iCurTraceSymbolsNum; //!<��ǰ��ջ���Ÿ���
		char m_szAllBackTraceStr[MAX_BACKTRACE_STRING_LENGTH]; //!<���ж�ջ�Ĵ�ӡ��Ϣ
	};

	//!һ����˵ֻ���õ�һ��CBacktraceInfo�࣬����ʵ�ֳɵ���
	typedef CSingleton<CBacktraceInfo> BacktraceSingleton;

}


#endif //__BACK_TRACE_INFO_HPP__
///:~
