#pragma once

class CCacheTime
{
public:
    void SetCreatedTime(int iTime);
    int GetCreatedTime();
    void GetCreatedTime(char* szTimeString);

protected:
	CCacheTime();

private:
	// �����㴴����ʱ��
	int m_iCreatedTime;
};
