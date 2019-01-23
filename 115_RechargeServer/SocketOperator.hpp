#pragma once

#include <netinet/in.h>

class CSocketOperator
{
public:
	//����
	static int Accept(const int iListeningFD, unsigned int& ruiIP, unsigned short& rushPort);
	//�ر�
	static int Close(const int iFD);

	//����
	static int Send(const int iFD, const int iCodeLength, const char* pszCodeBuffer);

	//����
	static int Recv(const int iFD, const int iCodeLength, const char* pszCodeBuffer);
};
