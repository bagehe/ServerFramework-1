#ifndef __SEQ_CONVERTER_HPP__
#define __SEQ_CONVERTER_HPP__

#define DAY_BASE 100000
#define WORLD_BASE 10

// ����һ��DBSeq��DBSeq = 100000 * unCreatedTime + 10 * nWorldID + nCacheSeq
unsigned int GenerateDBSeq(const unsigned short unCreatedTime,
                           const short nWorldID, const short nCacheSeq);
// ����DBSeq�е�CacheSeq��ֵ
short DBSeqToCacheSeq(const unsigned int uiDBSeq);
// ����DBSeq�е�WorldID��ֵ
short DBSeqToWorldID(const unsigned int uiDBSeq);
// ����DBSeq�е�CreatedTime��ֵ������ʾ��2011-1-1�������
unsigned short DBSeqToCreatedTime(const unsigned int uiDBSeq);
// ���DBSeq��������ɲ����Ƿ��ںϷ���Χ��
int CheckDBSeq(const unsigned int uiDBSeq);

#endif // __SEQ_CONVERTER_HPP__
