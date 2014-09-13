

#ifndef __MT_MESSAGE_PASSING__
#define __MT_MESSAGE_PASSING__

#include <windows.h>
#include <process.h>

///////////////////////////////////////////////////////////////////////////////
//#ifdef _M_X64
//typedef ULONG_PTR MYLONG_PTR;
//#else
//typedef PDWORD MYLONG_PTR;
//#endif


class CThreadPool 
{
public:
	typedef void (__stdcall *FnExecute)(DWORD, OVERLAPPED*);
	CThreadPool();

	~CThreadPool();
	
	LRESULT Begin
	(
		DWORD nMaxConcurrency=0 //number of threads to launch
	);

    BOOL PostMessage
    (
        FnExecute func,  // pointer to the call back function
        DWORD param,     // param  is normally cast from pointer to an object
        OVERLAPPED* pOv=NULL // pOv is usually null as worker threads do not need it
    );

    BOOL AssociateHandle
    (
        HANDLE hDevice,  // Handle to file or device
        FnExecute func   // call back function
    ) ;
   
private:
	BOOL CThreadPool::GetNextMessage
	(
		PDWORD pNumBytes, 
		PULONG_PTR pCompKey,
		OVERLAPPED** op, 
		DWORD dwMilliseconds = INFINITE
	); 
	static void __stdcall quit(DWORD,OVERLAPPED*);
	static UINT __stdcall MPConsumer(void* iocp);
 
	HANDLE m_port;
	HANDLE *thread;
	DWORD nThreads;
};


#endif