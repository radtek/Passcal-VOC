#pragma once

template<typename T>
class CValRst
{
public:
	CValRst(T * pVal, const T & RstVal, BOOL bEnable = TRUE)
		: m_pVal(pVal)
		, m_RstVal(RstVal)
		, m_bEnable(bEnable)
	{
	}

	~CValRst(void)
	{
		if (m_bEnable && (nullptr != m_pVal))
		{
			*m_pVal = m_RstVal;
		}
		
		m_bEnable = FALSE;
	}

public:
	void ResetValue(const T & RstVal)
	{
		m_RstVal = RstVal;
	}

	void Enable(void)
	{
		m_bEnable = TRUE;
	}

	void Disable(void)
	{
		m_bEnable = FALSE;
	}

protected:
	T *m_pVal;			// ����ָ��
	T m_RstVal;			// ����ֵ
	BOOL m_bEnable;		// ʹ��״̬
};


class CCallBackRst
{
	typedef void (*prst)(void);

public:
	CCallBackRst(prst pfn, BOOL bEnalbe = TRUE)
		: m_pFun(pfn)
		, m_bEnable(bEnalbe)
	{
	}

	~CCallBackRst(void)
	{
		if (m_bEnable && nullptr != m_pFun)
		{
			(*m_pFun)();
		}

		m_bEnable = FALSE;
	}

public:
	void Enable(void)
	{
		m_bEnable = TRUE;
	}

	void Disable(void)
	{
		m_bEnable = FALSE;
	}

protected:
	prst m_pFun;		// �ص�����ָ��
	BOOL m_bEnable;		// ʹ��״̬
};
