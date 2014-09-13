#include "WriteReq.h"
CWriteReq::CWriteReq (PVOID pBuffer, DWORD bufSize, HANDLE dstFile) 
	:
m_pvData(pBuffer),
m_nBuffSize(bufSize),
m_hFileDst(dstFile)
{
	Internal = InternalHigh = 0;   
	Offset = OffsetHigh = 0;   
	hEvent = NULL;
}

CWriteReq::~CWriteReq() 
{
	if (m_pvData != NULL)
		VirtualFree(m_pvData, 0, MEM_RELEASE);
}

BOOL CWriteReq::Write( DWORD off, DWORD offHi) 
{
	Offset     = off;
	OffsetHigh = offHi;
	return(::WriteFile(m_hFileDst, m_pvData, m_nBuffSize, NULL, this));
}

void __stdcall CWriteReq::WriteComplete(DWORD nCompleted, OVERLAPPED* pov)
{
   CWriteReq* pior=(CWriteReq*)pov;
   delete pior;  
}