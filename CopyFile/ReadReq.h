#ifndef __IOCP_READ_REQUEST__
#define __IOCP_READ_REQUEST__

#include <windows.h>
#include <process.h>
class CReadReq : public OVERLAPPED 
{
public:
	CReadReq(HANDLE sourceFile, HANDLE destFile, LONGLONG pos, DWORD bufferSize);

	~CReadReq();


	PVOID					DetachBuffer();
							
	BOOL					Read();
							
	DWORD					EndofFile();

	

private:
	BOOL AllocBuffer(DWORD nBuffSize);


public:
	LARGE_INTEGER	m_liNextReadOffset;
	LARGE_INTEGER	m_liFileSizeSrc;
	HANDLE			m_hFileSrc;
	HANDLE			m_hFileDst;
	DWORD			m_nBuffSize;
	PVOID			m_pvData;
};
#endif
