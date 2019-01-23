/**
*@file  CommonDef.hpp
*@author jasonxiong
*@date 2009-07-02
*@version 1.0
*@brief ���ļ�����һЩ�����ĺ궨��
*
*
*/

#ifndef __COMMON_DEF_HPP__
#define __COMMON_DEF_HPP__

#include "stddef.h"

namespace ServerLib
{
	const unsigned int MAX_FILENAME_LENGTH = 256; //!<�ļ�����󳤶�
	const unsigned int MAX_DATETIME_LENGTH = 32; //!<YYYY-mm-dd HH:MM:SS��ʽ��ʱ�䴮��󳤶�
	const unsigned int MAX_DATE_LENGTH = 16; //!<YYYY-mm-dd��ʽ��ʱ�䴮��󳤶�
	const unsigned int MAX_IPV4_LENGTH = 16; //!<IPv4���ַ�������
	const int MIN_TIME_STRING_LENGTH = 19; //!<����ת��ʱ���tmʱ���ַ���
	const int MAX_LINE_BUF_LENGTH = 1024; //!<һ���ַ�������󳤶�

	const int MICROSECOND_PER_SECOND = 1000000; //!<ÿ���ж���΢��

//!����|a-b|
#define ABS(a,b)  (((unsigned int) (a) > (unsigned int)(b)) ? ((a) - (b)) : ((b) - (a)))

//!��ȡa��b�Ľϴ���
#ifndef MAX
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#endif

	//!��ȡa��b�Ľ�С��
#ifndef MIN
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#endif

	//!��ȫ�ͷ�ָ��
#define DELETEOBJ(a)  {	if(a) { delete a; } a = NULL; }

	//!�ж�AWORD�Ƿ�����ĳһ��BIT
#define BIT_ENABLED(AWORD, BIT) (((AWORD) & (BIT)) != 0)

	//!�ж�AWORD�Ƿ�û����ĳһ��BIT
#define BIT_DISABLED(AWORD, BIT) (((AWORD) & (BIT)) == 0)

	//!��AWORD����ĳЩBITS
#define SET_BITS(AWORD, BITS) ((AWORD) |= (BITS))

	//!��AWORD���ĳЩBITS
#define CLR_BITS(AWORD, BITS) ((AWORD) &= ~(BITS))

	template <typename T, size_t N>
	char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))


	//��λ����������
#define cast(t, exp)	((t)(exp))
#define resetbits(x,m)	((x) &= (~(m)) )
#define setbits(x,m)	((x) |= (m))
#define testbits(x,m)	((x) & (m))
#define bitmask(b)	(1<<(b))

	//�����ֽ�x�ĵ�bλΪ1
#define setbitsbypos(x, b)		setbits(x, bitmask(b))
#define resetbitsbypos(x, b)	resetbits(x, bitmask(b));
}

#endif //__COMMON_DEF_HPP__
