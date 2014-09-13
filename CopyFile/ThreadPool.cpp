#include "ThreadPool.h"
void __stdcall CThreadPool::quit(DWORD,OVERLAPPED*)
{ 
	return;
}

UINT __stdcall CThreadPool::MPConsumer(void* lParam)
{
	CThreadPool* iocp=(CThreadPool*)lParam;
	ULONG_PTR CompKey=0;
	DWORD dwNumBytes=0;
	OVERLAPPED* ior=NULL;
	for(;;)
	{
		iocp->GetNextMessage( &dwNumBytes, &CompKey, &ior, INFINITE);

		FnExecute exec=reinterpret_cast<FnExecute>(CompKey);
		if(exec==quit)
			break;
		exec(dwNumBytes,ior);
	}

	iocp->PostMessage(quit,0);
	return 0;
}

CThreadPool::CThreadPool()
:
thread(NULL),
nThreads(0)
{
	m_port=NULL;
}
CThreadPool::~CThreadPool()
{
		if(m_port!=NULL)
		{
		PostMessage(quit, 0, NULL);
		WaitForMultipleObjects(nThreads,thread,TRUE,INFINITE);
		CloseHandle(m_port);
		delete [] thread;
		}
}
LRESULT CThreadPool::Begin(DWORD nMaxConcurrency)
{
	if(m_port==NULL)
	{
		if(nMaxConcurrency<=0)
			nThreads=2;
		else
			nThreads=nMaxConcurrency;
		m_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0,nThreads );
		//ToDo Error checking
		thread = new HANDLE[nThreads];
		for(DWORD i=0;i<nThreads;i++)
			thread[i]=(HANDLE)_beginthreadex(NULL,0,MPConsumer,this,0,NULL);
	}
	return GetLastError();
}


BOOL CThreadPool::PostMessage(FnExecute func, DWORD param, OVERLAPPED* por/*=NULL*/) 
{
	ULONG CompKey=(DWORD)func;
	BOOL fOk = PostQueuedCompletionStatus(m_port, param, CompKey, por);
	return(fOk);
}
BOOL CThreadPool::AssociateHandle(HANDLE hDevice, FnExecute func) 
{
	return	CreateIoCompletionPort(hDevice, m_port, (ULONG_PTR)func, 0) 
			== 
			m_port;
}

BOOL CThreadPool::GetNextMessage
(
	PDWORD pNumBytes, 
	PULONG_PTR pCompKey,
	OVERLAPPED** op, 
	DWORD dwMilliseconds /*= INFINITE*/
) 
{
	return GetQueuedCompletionStatus(m_port, pNumBytes, pCompKey, op, dwMilliseconds);
}

