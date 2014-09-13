#ifndef __IOCP_WRITE_REQ
#define __IOCP_WRITE_REQ
#include <windows.h>
#include <process.h>
// Each I/O Request needs an OVERLAPPED structure and a data buffer
class CWriteReq : public OVERLAPPED {
public:
	CWriteReq(PVOID pBuffer, DWORD bufSize, HANDLE dstFile);

	~CWriteReq() ;

	BOOL Write( DWORD off, DWORD offHi);



private:	
	PVOID  m_pvData;
	DWORD m_nBuffSize;
	HANDLE m_hFileDst;
};
#endif