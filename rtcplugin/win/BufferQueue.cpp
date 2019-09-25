#include "BufferQueue.h"
#include <Windows.h>

CBufferQueue::CBufferQueue()
{
	
}


CBufferQueue::~CBufferQueue()
{
	
}


bool CBufferQueue::Create(int nUnitCount, uint32_t nBytesPreUnit)
{
	if (nUnitCount < 0 || nBytesPreUnit == 0)
		return false;

	// �Ѿ��������
	if (!m_listFreeUnit.empty())
		return true;

	uint8_t*	lpBuffer = NULL;

	m_csListLock.lock() ;

	// ��ʼ���ڴ��
	for (int nIndex = 0; nIndex < nUnitCount; nIndex++){
		lpBuffer = new uint8_t[nBytesPreUnit];
		m_listFreeUnit.push_back(lpBuffer);
	}

	m_csListLock.unlock() ;

	m_nUnitCount = nUnitCount;
	m_nCurrentCount = nUnitCount;
	m_nBytesPreUnit = nBytesPreUnit;

	return true;
}

bool CBufferQueue::Close()
{
	uint8_t* lpBuffer = NULL;

	m_csListLock.lock() ;

	if (!m_listBusyUnit.empty()){			// ����æ�飬��ֹ�ͷ�
		m_csListLock.unlock() ;
		return false;
	}

	while (m_listFreeUnit.size() > 0){
		lpBuffer = m_listFreeUnit.front() ;
		m_listFreeUnit.pop_front() ;
		delete[] lpBuffer;
	}

	m_nBytesPreUnit = 0;
	m_nUnitCount = 0;
	m_nCurrentCount = 0;

	m_csListLock.unlock() ;		// ���ڻ���ȫ��������ϣ��˳��ٽ�

	return true;
}

int	CBufferQueue::GetFreeCount() const
{
	return (int)m_listFreeUnit.size();
}

int	CBufferQueue::GetBusyCount() const
{
	return (int)m_listBusyUnit.size();
}

void*	CBufferQueue::AllocBuffer(bool bForceAlloc)
{
	uint8_t*		lpBuffer = NULL;	// ������

	m_csListLock.lock() ;
	std::list<uint8_t*>::iterator posHead ;		// ����ͷ
	std::list<uint8_t*>::iterator posTail; 		// ����β

	posHead = m_listFreeUnit.begin() ;
	posTail = m_listFreeUnit.end() ;

	if (posHead != posTail)
	{
		lpBuffer = m_listFreeUnit.front();
		m_listFreeUnit.pop_front() ;
	}
	


	// ::EnterCriticalSection(&m_csListLock);				// ��������ٽ�
	// posHead = m_listFreeUnit.be();			// ��������Ƿ�Ϊ��
	// posTail = m_listFreeUnit.back();			// ����βҲҪ���

	// if (posHead != NULL && posHead != posTail)			// ����ǿ���ͷβ��ͬ
	// 	lpBuffer = m_listFreeUnit.RemoveHead();
	// else if (posHead != NULL && posHead == posTail)		// ֻ��һ������
	// 	lpBuffer = m_listFreeUnit.RemoveHead();
	// else{												// �����޿���
	// 	if (bForceAlloc){								// ����ǿ�Ʒ���
	// 		lpBuffer = new BYTE[m_nBytesPreUnit];
	// 		m_nCurrentCount++;
	// 	}
	// 	else
	// 		lpBuffer = NULL;
	// }
	//m_csListLock.lock() ;
	//lpBuffer = new BYTE[m_nBytesPreUnit];
	//m_nCurrentCount++;

	if (lpBuffer == NULL) {
		m_csListLock.unlock() ;
		return NULL;
	}

	m_listBusyUnit.push_back(lpBuffer);
	m_csListLock.unlock() ;

	return lpBuffer;
}

bool CBufferQueue::FreeBusyHead(void* lpDestBuf, uint32_t nBytesToCpoy)
{
	bool bRet = FALSE;
	uint8_t* lpBuffer = NULL;

	m_csListLock.lock() ;
	if (!m_listBusyUnit.empty()) {
		lpBuffer = m_listBusyUnit.front();
		m_listBusyUnit.pop_front() ;
	}

	if (lpDestBuf != NULL && lpBuffer != NULL)
		memcpy(lpDestBuf, lpBuffer, nBytesToCpoy);

	if (lpBuffer != NULL)
		m_listFreeUnit.push_back(lpBuffer);

	m_csListLock.unlock() ;

	return lpBuffer != NULL ? true : false;
}

void CBufferQueue::FreeAllBusyBlock()
{
	bool bRet = TRUE;

	do {
		bRet = FreeBusyHead(NULL, 0);
	} while (!bRet);
}