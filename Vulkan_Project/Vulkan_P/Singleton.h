#pragma once
#include "stdafx.h"
#include "Object.h"
/*
---------------------------------------------------------------------------------------
�̱��� ���ø� Ŭ����
---------------------------------------------------------------------------------------
*/

template <class ClassName>
class CSingleTonBase : public Object
{
private:
	static ClassName* m_sInstance;


protected:

	CSingleTonBase(std::string name) : Object(name) {}

	~CSingleTonBase() {};


public:
	static ClassName* GetInstance()
	{
		if (nullptr == m_sInstance)
			return m_sInstance = new ClassName;

		return m_sInstance;
	}

	void ReleseInstance()
	{
		if (m_sInstance)
		{
			delete m_sInstance;
			m_sInstance = nullptr;
		}
	}
};

//�̱��� �ν��Ͻ� �ʱ�ȭ
template <class ClassName>
ClassName* CSingleTonBase<ClassName>::m_sInstance = nullptr;
