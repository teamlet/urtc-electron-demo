#pragma once

#include <stdint.h>
#include <list>
#include <cstddef>
#include <mutex>

class CBufferQueue
{
public:
	CBufferQueue();
	~CBufferQueue();

	bool Create(int nUnitCount, uint32_t nBytesPreUnit);
	bool Close();

	uint32_t	GetBytesPreUnit() const { return m_nBytesPreUnit; };
	int		GetUnitCount() const { return m_nUnitCount; };
	int		GetCurrentCount() const { return m_nCurrentCount; };

	int		GetFreeCount() const;
	int		GetBusyCount() const;

	void*	AllocBuffer(bool bForceAlloc = false);

	bool	FreeBusyHead(void* lpDestBuf, uint32_t nBytesToCpoy);
	void	FreeAllBusyBlock();

private:
	int			m_nUnitCount;		// ��Ԫ��ʼֵ
	int			m_nCurrentCount;	// ��ǰ����Ŀ
	uint32_t		m_nBytesPreUnit;

	std::list<uint8_t*>	m_listFreeUnit;	// ���õ�Ԫ
	std::list<uint8_t*>	m_listBusyUnit;	// �����õ�Ԫ

	std::mutex   m_csListLock;	// æ����
};

