
#include <fstream>
#include <sstream>

#include "pugixml.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"

#include "MaskWordConfig.hpp"

using namespace std;
using namespace ServerLib;
using namespace pugi;

MaskWordConfig::MaskWordConfig()
{
	Reset();
}

MaskWordConfig::~MaskWordConfig()
{
	Reset();
}

int MaskWordConfig::LoadConfig(const char* szFile)
{
	if (!szFile)
	{
		return -1;
	}

	//���ļ�
	xml_document oXmlDoc;
	xml_parse_result oResult = oXmlDoc.load_file(szFile);
	if (!oResult)
	{
		LOGERROR("Failed to open xml config file : %s\n", szFile);
		return -2;
	}

	Reset();

	//��������������
	std::string strMaskWord;
	MaskNode* pCurrentNode = &m_astNodes[m_iUsedNum++];	//��ͷ��ʼ

	xml_node oRootNode = oXmlDoc.child("maskwords");
	for (xml_node oOneNode = oRootNode.first_child(); oOneNode; oOneNode = oOneNode.next_sibling())
	{
		strMaskWord = oOneNode.text().as_string();

		for (unsigned j = 0; j < strMaskWord.size(); ++j)
		{
			//�Ƿ��Ѿ����ڣ�����Sibling�ڵ�
			MaskNode* pNode = IsNodeExist(pCurrentNode->pNextNode, strMaskWord[j]);

			if (!pNode) {
				pNode = &m_astNodes[m_iUsedNum++];	//��ȡ�½ڵ�
				pNode->cData = strMaskWord[j];
				pNode->bIsLast = false;
				pNode->pNextNode = NULL;
				pNode->pSiblingNode = NULL;

				if (!pCurrentNode->pNextNode)
				{
					pCurrentNode->pNextNode = pNode;
				}
				else 
				{
					MaskNode* pstTempNode = pCurrentNode->pNextNode;
					while (pstTempNode->pSiblingNode)
					{
						pstTempNode = pstTempNode->pSiblingNode;
					}

					pstTempNode->pSiblingNode = pNode;
				}
			}

			pCurrentNode = pNode; 
		}

		pCurrentNode->bIsLast = true;
		pCurrentNode = &m_astNodes[0];
	}

	return 0;
}

//�Ƿ����������
bool MaskWordConfig::HasMaskWord(const std::string& strMaskWord)
{
	MaskNode* pCurrentNode = m_astNodes[0].pNextNode;
	int iStartIndex = -1;
	stringstream ss;
	bool bNeedReview = false;

	for (unsigned i = 0; i < strMaskWord.size(); ++i)
	{
		bool bHasFind = false;
		char cData = strMaskWord[i];

		while (pCurrentNode)
		{
			if (pCurrentNode->cData == cData)
			{
				//�����ַ�ƥ��
				bHasFind = true;
				if (iStartIndex == -1)
				{
					iStartIndex = i;
				}

				if (pCurrentNode->bIsLast && !pCurrentNode->pNextNode)
				{
					return true;

					/*
					iStartIndex = -1;
					pCurrentNode = m_astNodes[0].pNextNode;
					bNeedReview = false;
					*/
				}
				else if (pCurrentNode->bIsLast)
				{
					return true;

					/*
					iStartIndex = (i+1);
					pCurrentNode = pCurrentNode->pNextNode;
					bNeedReview = true;
					*/
				}
				else
				{
					pCurrentNode = pCurrentNode->pNextNode;
				}

				break;
			}

			pCurrentNode = pCurrentNode->pSiblingNode;
		}

		if (bNeedReview && !bHasFind)
		{
			i = iStartIndex - 1;
			bNeedReview = false;
			iStartIndex = -1;
			pCurrentNode = m_astNodes[0].pNextNode;
		}
		else if (!bHasFind)
		{
			if (iStartIndex > -1)
			{
				i = iStartIndex;
			}

			iStartIndex = -1;
			pCurrentNode = m_astNodes[0].pNextNode;
		}
	}

	return false;
}

//�ڵ��Ƿ��Ѵ���
MaskNode* MaskWordConfig::IsNodeExist(MaskNode* pCurrentNode, char cData)
{
	while (pCurrentNode)
	{
		if (pCurrentNode->cData == cData)
		{
			//����
			return pCurrentNode;
		}

		pCurrentNode = pCurrentNode->pSiblingNode;
	}

	return NULL;
}

//����
void MaskWordConfig::Reset()
{
	m_iUsedNum = 0;

	memset(m_astNodes, 0, sizeof(m_astNodes));
}
