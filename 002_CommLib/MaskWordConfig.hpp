#pragma once

#include <string>
#include <string.h>

#include "json/value.h"

//����������

//�����ֽڵ�����
static const int MAX_MASKWORD_NODE_NUM = 20000;

struct MaskNode
{
	char cData;				//�ַ�
	bool bIsLast;			//�Ƿ����һ��
	MaskNode* pSiblingNode;	//�ֵܽڵ�
	MaskNode* pNextNode;	//��һ���ڵ�

	MaskNode()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

class MaskWordConfig
{
public:
	MaskWordConfig();
	virtual ~MaskWordConfig();

	//����������
	int LoadConfig(const char* szFile);

public:

	//�Ƿ����������
	bool HasMaskWord(const std::string& strMaskWord);

	//����
	void Reset();

private:

	//�ڵ��Ƿ��Ѵ���
	MaskNode* IsNodeExist(MaskNode* pCurrentNode, char cData);

private:

	//��ǰ��ʹ�ýڵ�����
	int m_iUsedNum;

	//�����ֽڵ�
	MaskNode m_astNodes[MAX_MASKWORD_NODE_NUM];
};
