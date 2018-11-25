#pragma once

#include <vector>

enum
{
	 NEW_PTR_MODE_DEC = 0,
	 NEW_PTR_MODE_HEX,
	 NEW_PTR_MODE_SUM
};

template<typename T>
class CNewPtr
{
public:
	CNewPtr(void)
		: m_pNew(nullptr)
		, m_nSize(0)
		, m_nRealSize(0)
	{
	}

	CNewPtr(int nNew)
	{
		ASSERT(nNew >= 0);

		m_nSize = nNew;
		m_nRealSize = nNew;
		m_pNew = new T[nNew];
	}

	CNewPtr(int nNew, T nVal)
	{
		ASSERT(nNew >= 0);

		m_nSize = nNew;
		m_nRealSize = nNew;
		m_pNew = new T[nNew];

		memset(m_pNew, nVal, m_nSize * sizeof(T));
	}

	CNewPtr(CNewPtr<T> * p)
	{
		m_nSize = p->m_nSize;
		m_nRealSize = m_nSize;
		m_pNew = new T[m_nSize];

		memcpy(m_pNew, p->m_pNew, m_nSize * sizeof(T));
	}

	~CNewPtr(void)
	{
		Delete();
	}

public:
	T *m_pNew;	
	int m_nSize;
private:
	int m_nRealSize;
public:

	void Delete(void)
	{
		m_nSize = 0;
		m_nRealSize = 0;

		if (nullptr != m_pNew)
		{
			delete []m_pNew;
			m_pNew = nullptr;
		}
	}

	void ReNew(int nSize)
	{
		ASSERT(nSize >= 0);

		Delete();

		m_nSize = nSize;
		m_nRealSize = nSize;
		m_pNew = new T[m_nSize];
	}

	void ReNew(int nSize, T nVal)
	{
		ASSERT(nSize >= 0);

		Delete();

		m_nSize = nSize;
		m_nRealSize = nSize;
		m_pNew = new T[nSize];

		memset(m_pNew, nVal, m_nSize * sizeof(T));
	}

	void ReNew2(int nSize, ...)
	{
		ASSERT(nSize >= 0);

		Delete();

		m_nSize = nSize;
		m_nRealSize = nSize;
		m_pNew = new T[m_nSize];

		va_list ap = nullptr;
		va_start(ap, nSize);

		for (int i = 0; i < nSize; i++)
		{
			const T t = va_arg(ap, T);

			m_pNew[i] = t;
		}

		va_end(ap);
	}

	int A2I(int nStart, int nLength, int nMode)
	{
		ASSERT(nStart >= 0 && nStart + nLength >= 0 && nStart + nLength <= m_nSize);

		if (nLength < 0)
		{
			nStart += nLength;
			nLength *= -1;
		}

		int nRetVal = 0;

		if (NEW_PTR_MODE_DEC == nMode)
		{
			nLength += nStart;

			for(int i = nStart; i < nLength; i++)
			{
				if (m_pNew[i] >= '0' && m_pNew[i] <= '9')
				{
					const int t = m_pNew[i] - '0';

					nRetVal *= 10;
					nRetVal += t;
				}
			}
		}
		else if (NEW_PTR_MODE_HEX == nMode)
		{
			nLength += nStart;

			for(int i = nStart; i < nLength; i++)
			{
				int t = 0;

				if (m_pNew[i] >= '0' && m_pNew[i] <= '9')
				{
					t = m_pNew[i] - '0';

					nRetVal <<= 4;
					nRetVal += t;
				}
				else if ((m_pNew[i] | 32) >= 'a' && (m_pNew[i] | 32) <= 'f')
				{
					t = (m_pNew[i] | 32) - 'a' + 10;

					nRetVal <<= 4;
					nRetVal += t;
				}
			}
		}

		return nRetVal;
	}

	int Hex2I(int nStart, int nLength)
	{
		ASSERT(nStart >= 0 && nStart + nLength >= 0 && nStart + nLength <= m_nSize);

		if (nLength < 0)
		{
			nStart += nLength;
			nLength *= -1;
		}

		nLength += nStart;

		int nRetVal = 0;

		for (int i = nStart; i < nLength; i++)
		{
			nRetVal <<= 8;
			nRetVal += (BYTE)m_pNew[i];
		}

		return nRetVal;
	}

	T & at(int nPos)
	{
		ASSERT(abs(nPos) <= max(m_nSize, m_nRealSize));

		if (nPos < 0)
		{
			nPos += m_nSize;
		}
		
		return m_pNew[nPos];
	}

	void Append(const T & t)
	{
		CNewPtr<T> NewTmp(this);

		ReNew(m_nSize + 1);

		memcpy(m_pNew, NewTmp.m_pNew, sizeof(T) * NewTmp.m_nSize);

		m_pNew[NewTmp.m_nSize] = t;
	}

	void CopyTo(std::vector<T> & v)
	{
		v.resize(m_nSize);

		for (int i = 0; i < m_nSize; i++)
		{
			v[i] = m_pNew[i];
		}
	}

	void CloneFrom(const std::vector<T> & v)
	{
		ReNew(v.size());

		for (int i = 0; i < m_nSize; i++)
		{
			m_pNew[i] = v[i];
		}
	}

	void CloneFrom(const CString & s)
	{
		USES_CONVERSION;

		const char *p = W2A(s);
		const int nLength = strlen(p);

		ReNew(nLength + 1);
		m_nSize--;

		memcpy(m_pNew, p, nLength);

		m_pNew[m_nSize] = 0;
	}
	
	void Trace(void)
	{
#ifdef _DEBUG

		TRACE(_T("\n------------------------------------"));

		for (int i = 0; i < m_nSize; i++)
		{
			TRACE(_T("\n0x%02X(%d)"), (T)m_pNew[i], (T)m_pNew[i]);
		}
#endif // _DEBUG
	}

	void Trace(int nStart, int nLength)
	{
#ifdef _DEBUG

		TRACE(_T("\n------------------------------------"));

		ASSERT(nStart >= 0 && nStart + nLength >= 0 && nStart + nLength <= m_nSize);

		if (nLength < 0)
		{
			const int nEnd = nStart - nLength;

			for (int i = nStart; i > nEnd; i--)
			{
				TRACE(_T("\n0x%02X(%d)"), (T)m_pNew[i], (T)m_pNew[i]);
			}
		}
		else
		{
			nLength += nStart;

			for (int i = nStart; i < nLength; i++)
			{
				TRACE(_T("\n0x%02X(%d)"), (T)m_pNew[i], (T)m_pNew[i]);
			}
		}
#endif // _DEBUG
	}

	void operator =(const CNewPtr & p)
	{
		Delete();

		m_nSize = p.m_nSize;
		m_nRealSize = m_nSize;
		m_pNew = new T[m_nSize];

		memcpy(m_pNew, p.m_pNew, m_nSize * sizeof(T));
	}

	T & operator [](int nPos)
	{
		ASSERT(abs(nPos) <= max(m_nSize, m_nRealSize));

		if (nPos < 0)
		{
			nPos += m_nSize;
		}

		return m_pNew[nPos];
	}

	CNewPtr<T> operator + (const T & t)
	{
		CNewPtr<T> NewTmp(this);

		ReNew(m_nSize + 1);

		memcpy(m_pNew, NewTmp.m_pNew, sizeof(T) * NewTmp.m_nSize);

		m_pNew[NewTmp.m_nSize] = t;

		return this;
	}

	void operator += (const T & t)
	{
		CNewPtr<T> NewTmp(this);

		ReNew(m_nSize + 1);

		memcpy(m_pNew, NewTmp.m_pNew, sizeof(T) * NewTmp.m_nSize);

		m_pNew[NewTmp.m_nSize] = t;
	}

	CNewPtr<T> operator + (const CNewPtr<T> & t)
	{
		CNewPtr<T> NewTmp(this);

		ReNew(m_nSize + t.m_nSize);

		memcpy(m_pNew, NewTmp.m_pNew, sizeof(T) * NewTmp.m_nSize);
		memcpy(m_pNew + NewTmp.m_nSize, t.m_pNew, sizeof(T) * t.m_nSize);

		return this;
	}

	void operator += (const CNewPtr<T> & t)
	{
		CNewPtr<T> NewTmp(this);

		ReNew(m_nSize + t.m_nSize);

		memcpy(m_pNew, NewTmp.m_pNew, sizeof(T) * NewTmp.m_nSize);
		memcpy(m_pNew + NewTmp.m_nSize, t.m_pNew, sizeof(T) * t.m_nSize);
	}
};
