#include "DebugManager.h"
#include "EventManager.h"

#include <iostream>
#include <fstream>

int CDebugManager::m_nLineCount;
TDebugLineMessage CDebugManager::m_tDebugLines[MAX_DEBUG_LINES];
void CDebugManager::DebugLog(TDebugMessage Message)
{
	std::cout << Message.m_szMessageString << std::endl;
}

void CDebugManager::AddDebugVertex(TDebugLineMessage Message)
{
	
	if(m_nLineCount == MAX_DEBUG_LINES)
	{
		std::cout << "Debug Lines limit reached." << std::endl;
		return;
	}
	m_tDebugLines[m_nLineCount++] = Message;
}

void* CDebugManager::GetDebugLineVertices()
{
	return &m_tDebugLines;
}

int CDebugManager::GetDebugVertexCount()
{
	return m_nLineCount;
}

void CDebugManager::ClearDebugLines()
{
	m_nLineCount = 0;
}

LONG __stdcall CDebugManager::errorFunc(struct _EXCEPTION_POINTERS * pExceptionInfo)
{
	struct tm newTime;
	time_t ltime;
	wchar_t buff[100] = { 0 };

	ltime = time(&ltime);
	localtime_s(&newTime, &ltime);

	wcsftime(buff, sizeof(buff), L"%A_%b%d_%I%M%p.mdmp", &newTime);


	HANDLE hFile = CreateFileW(L"dumpfile.mdmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		//MessageBox((HWIND)"Dump File Saved look x directory please email to developer at the following email adress crashdmp@gmail.com with the subject Gamename - Version ");
		CloseHandle(hFile);
	}

	return 0;

}

