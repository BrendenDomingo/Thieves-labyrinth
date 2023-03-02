#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Dbghelp.h>
#include <time.h>
#include "Math.h"
#include "System.h"

#include <vector>

#define MAX_DEBUG_LINES 10000

struct TDebugMessage;
struct TDebugLineMessage;

class CDebugManager : public ISystem
{
public:


	static void DebugLog(TDebugMessage tMessage);

	static void AddDebugVertex(TDebugLineMessage tMessage);

	static void* GetDebugLineVertices();

	static int	GetDebugVertexCount();

	static void ClearDebugLines();

	static LONG WINAPI errorFunc(struct _EXCEPTION_POINTERS *pExceptionInfo);

private:

	static int			m_nLineCount;
	
	static TDebugLineMessage m_tDebugLines[MAX_DEBUG_LINES];
};

