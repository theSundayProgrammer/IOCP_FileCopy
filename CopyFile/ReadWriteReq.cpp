
#include "ReadReq.h"
#include "WriteReq.h"



CReadWriteReq::CReadWriteReq(HANDLE sourceFile, HANDLE destFile, LONGLONG pos, DWORD bufferSize)
	:
	m_hFileSrc(sourceFile),
	m_hFileDst(destFile)
{
	Internal = InternalHigh = 0;
	hEvent = NULL;
	Offset = OffsetHigh = 0;
	m_nBuffSize = 0;
	m_pvData = NULL;
	m_liNextReadOffset.QuadPart = pos;
	AllocBuffer(bufferSize);

}

CReadWriteReq::~CReadWriteReq()
{
	if (m_pvData != NULL)
		VirtualFree(m_pvData, 0, MEM_RELEASE);
}

BOOL CReadWriteReq::AllocBuffer(DWORD nBuffSize)
{
	m_nBuffSize = nBuffSize;
	m_pvData = VirtualAlloc(NULL, m_nBuffSize, MEM_COMMIT, PAGE_READWRITE);
	return(m_pvData != NULL);
}


BOOL CReadWriteReq::Read()
{
	Internal = InternalHigh = 0;
	hEvent = NULL;

	Offset = m_liNextReadOffset.LowPart;
	OffsetHigh = m_liNextReadOffset.HighPart;
	return(::ReadFile(m_hFileSrc, m_pvData, m_nBuffSize, NULL, this));
}

DWORD CReadWriteReq::EndofFile()
{
	return  m_liNextReadOffset.QuadPart >= m_liFileSizeSrc.QuadPart;
}


