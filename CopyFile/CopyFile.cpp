// CopyFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ThreadPool.h"
#include "ReadReq.h"
#include "WriteReq.h"
#include <windows.h>
#include <stdio.h>
typedef DWORD(WINAPI threadproc)(void*);
const int	MAX_THREADS = 2;
const DWORD BUFFSIZE = (8 * 1024);  // The size of an I/O buffer
const ULONG MAX_PENDING_TASKS = 100;
// roundup 
// returns  min{k: k >= Value && k%Multiple == 0}
template<typename T, typename S>
T roundup(T Value, S Multiple)
{
	T retVal = (Value / Multiple) * Multiple;
	//Invariant retVal % Multiple == 0 
	while (retVal < Value) 
		retVal += Multiple;
	return retVal;
}
//
//CCloseFile is a class that closes a file handle on destruction
//
//In production code one would use boost -scoped or shared pointers
//See www.boost.org for further details
//
class CCloseFile
{
public:
	CCloseFile(HANDLE h)
		: m_h(h){}
	~CCloseFile()
	{
		if (m_h)
			CloseHandle(m_h);
	}
	operator HANDLE()
	{
		return m_h;
	}
private:
	HANDLE m_h;


};
ULONG pendingTasks = 0;

void __stdcall ReadComplete(DWORD nCompleted, OVERLAPPED* pov)
{
	//write what we've got 
	CReadReq* pior = (CReadReq*)pov;
	CWriteReq* pwr = new CWriteReq(pior->DetachBuffer(), pior->m_nBuffSize, pior->m_hFileDst);
	pwr->Write(pov->Offset, pov->OffsetHigh);
	delete pior;
}

void __stdcall WriteComplete(DWORD nCompleted, OVERLAPPED* pov)
{
	CWriteReq* pior = (CWriteReq*)pov;
	delete pior;
	::InterlockedDecrement(&pendingTasks);
}

int _tmain(int argc, _TCHAR* argv[])
{
	LARGE_INTEGER liFileSizeSrc;
	if (argc<3)
	{
		printf("Usage: %s DestinationFile SourceFile\n", argv[0]);
		return 0;
	}
	DWORD k = GetTickCount();
	{
		//Open Source File
		//Create Destination file
		//Set destination file size to multiple of BUFFSIZE 
		//	but greater than or equal to Source File size
		CCloseFile hfileSrc = CreateFile
			(
			argv[2],
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
			NULL
			);
		if (hfileSrc == INVALID_HANDLE_VALUE)
		{
			printf("invalid source file\n");
			return 1;
		}
		GetFileSizeEx(hfileSrc, &liFileSizeSrc);

		CCloseFile hfileDst = CreateFile
			(
			argv[1],
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
			hfileSrc
			);
		if (hfileDst == INVALID_HANDLE_VALUE)
		{
			printf("invalid destination file\n");
			return 2;
		}

		LARGE_INTEGER liFileSizeDst;
		liFileSizeDst.QuadPart = roundup(liFileSizeSrc.QuadPart, BUFFSIZE);
		SetFilePointerEx(hfileDst, liFileSizeDst, NULL, FILE_BEGIN);
		SetEndOfFile(hfileDst);

		//Create thread pool with IO completion port
		//Associate source and destination file with IO completion port
		CThreadPool iocp;
		iocp.Begin(MAX_THREADS);
		iocp.AssociateHandle(hfileSrc, ReadComplete);
		iocp.AssociateHandle(hfileDst, WriteComplete);

		//launch read operations for each block
		//CReadReq::ReadComplete launches a write
		for (LONGLONG pos = 0; pos<liFileSizeSrc.QuadPart; pos += BUFFSIZE)
		{
			InterlockedIncrement(&pendingTasks);
			CReadReq* reqRead = new CReadReq(hfileSrc, hfileDst, pos, BUFFSIZE);
			reqRead->Read();
			while (pendingTasks > MAX_PENDING_TASKS) Sleep(0);
		}
	}
	{
		//Open destination file again and truncate its size
		CCloseFile hfileDst = CreateFile
			(
			argv[1],
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
			);
		if (hfileDst != INVALID_HANDLE_VALUE)
		{

			SetFilePointerEx(hfileDst, liFileSizeSrc, NULL, FILE_BEGIN);
			SetEndOfFile(hfileDst);
		}
	}
	printf("time taken=%d\n", GetTickCount() - k);
	return 0;
}
//Acknowledgements:
//This program was inspired by FileCopy
//from "Programming Server Side Applications
//by  Jeffrey Richter & Jason Clark


//int _tmain(int argc, _TCHAR* argv[])
//{
//	printf("hello world\n");
//	return 0;
//}

