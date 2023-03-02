//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#include <random>
#include <time.h>
#include <iostream>

#include "DebugManager.h"
#include "GameLogic.h"

#pragma comment (lib,"Dbghelp.lib")

static TCHAR szWindowClass[] = _T("Thieves Labyrinth");
static TCHAR szTitle[] = _T("Thieves Labyrinth");

HINSTANCE hInst;

#define FULLSCREEN 0
#define CONSOLE 1

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned int)time(0));

	SetUnhandledExceptionFilter(&CDebugManager::errorFunc);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(230574);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	WNDCLASSEX windowDesc;
	windowDesc.cbSize = sizeof(WNDCLASSEX);
	windowDesc.style = CS_HREDRAW | CS_VREDRAW;
	windowDesc.lpfnWndProc = CInputManager::WndProc;
	windowDesc.cbClsExtra = 0;
	windowDesc.cbWndExtra = 0;
	windowDesc.hInstance = hInstance;
	windowDesc.hIcon = (HICON)LoadImage(NULL, L"../Assets/Logo.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	windowDesc.hCursor = LoadCursorFromFile(L"../Assets/GameCursor.cur");
	windowDesc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowDesc.lpszMenuName = NULL;
	windowDesc.lpszClassName = szWindowClass;
	windowDesc.hIconSm = LoadIcon(windowDesc.hInstance, IDI_ASTERISK);


	if (!RegisterClassEx(&windowDesc))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}

	hInst = hInstance;

	#if CONSOLE

	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "I can smell the debug:" << std::endl;

	#endif // CONSOLE


	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	long type = 0;
	#if FULLSCREEN
	type = WS_POPUP;
	#else
	type = WS_OVERLAPPEDWINDOW;
	w /= 2;
	h /= 2;
	#endif


	HWND windowHandle = CreateWindow(szWindowClass, szTitle,
		type,
		CW_USEDEFAULT, CW_USEDEFAULT,
		w, h,
		NULL, NULL, hInstance, NULL);

	if (!windowHandle)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}

	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	ShowWindow(windowHandle, 
	#if FULLSCREEN
	w / h
	#else
	SW_SHOW
	#endif
	);
	UpdateWindow(windowHandle);


	CGameLogic* pcGameLogic = new CGameLogic(windowHandle);

	MSG msg;
	while (true)
	{
		//GetMessage(&msg, NULL, 0, 0);
		// Process all messages, stop on WM_QUIT 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
		else
		{
			pcGameLogic->Update();
			CInputManager::UpdatePresses();
		}
	}

	delete pcGameLogic;
	//_CrtDumpMemoryLeaks();
	return (int)msg.wParam;
}