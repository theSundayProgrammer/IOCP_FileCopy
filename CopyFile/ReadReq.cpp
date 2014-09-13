
#include "ReadReq.h"
#include "WriteReq.h"
CReadReq::CReadReq (HANDLE sourceFile, HANDLE destFile, LONGLONG pos, DWORD bufferSize)
	:
m_hFileSrc(sourceFile),
m_hFileDst(destFile)
{
	Internal = InternalHigh = 0;   
	hEvent = NULL;
	Offset = OffsetHigh = 0;   
	m_nBuffSize = 0;
	m_pvData = NULL;
	m_liNextReadOffset.QuadPart=pos;
	AllocBuffer(bufferSize);

}

CReadReq::~CReadReq() 
{
    if (m_pvData != NULL)
        VirtualFree(m_pvData, 0, MEM_RELEASE);
}

BOOL CReadReq::AllocBuffer(DWORD nBuffSize) 
{
    m_nBuffSize = nBuffSize;
    m_pvData = VirtualAlloc(NULL, m_nBuffSize, MEM_COMMIT, PAGE_READWRITE);
    return(m_pvData != NULL);
}
PVOID CReadReq::DetachBuffer()
{
	PVOID p=m_pvData;
	m_pvData=NULL;
	return p;
}

BOOL CReadReq::Read(  ) 
{
	Internal = InternalHigh = 0;   
	hEvent = NULL;

	Offset     = m_liNextReadOffset.LowPart;
	OffsetHigh = m_liNextReadOffset.HighPart;
	return(::ReadFile(m_hFileSrc, m_pvData, m_nBuffSize, NULL, this));
}

DWORD CReadReq::EndofFile()
{
	return  m_liNextReadOffset.QuadPart >= m_liFileSizeSrc.QuadPart;
}


