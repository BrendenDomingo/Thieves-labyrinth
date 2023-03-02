#pragma once
#include "System.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <bitset>
#include "Math.h"

class CTransform;

#define NBITSIZE 53

class CInputManager : public ISystem
{
	static std::bitset<NBITSIZE> m_bKeysDown;
	static std::bitset<NBITSIZE> m_bKeys;
	static std::bitset<NBITSIZE> m_bKeyBools;
	static int	nMouseXPos, nMouseYPos, nScreenWidth, nScreenHeight, nXDiff, nYDiff;
	static const CMath::TMATRIX* m_pProjMat;
	static CTransform* m_pCameraTransform;
	static RECT m_WindowBox;
	static bool m_bUpdateScroll;
	static int m_nScrollDirection;

public:
	// Checks to see if a key is down
	// Works on the frame it is pressed
	// chKey: Key to get
	// NOTE: Alphabet letters should be capitalized
	static bool GetKeyDown(char chKey);

	// Checks to see if a key is held
	// Works constantly until the key is released
	// chKey: Key to get
	// NOTE: Alphabet letters should be capitalized
	static bool GetKey(char chKey);

	// Get the mouse positions to be calculated to world space
	// nMouseX: integer to take the mouse's x position
	// nMouseY: integer to take the mouse's y position
	static void GetMousePos(int& nMouseX, int& nMouseY);

	// Get the mouse position converted to NDC space
	// _nX: integer to take the mouse's x position in NDC space
	// _nY: integer to take the mouse's y position in NDC space
	static void GetMousePosNDC(float& _nX, float& _nY);

	// Get the difference in mouse position
	// nDiffX: integer to take the mouse's x difference
	// nDiffY: integer to take the mouse's y difference
	static void GetMouseDelta(int& nDiffX, int& nDiffY);

	// Get the window width and height
	// nWidth: integer to take the window's width
	// nHeight: integer to take the window's height
	static void GetScreenWH(int& nWidth, int& nHeight);

	// Sets the view matrix that InputManager is pointing to
	// _mView: matrix to point to
	static void SetViewTransform(CTransform& _mView);
	   	 
	// Sets the proj matrix that InputManager is pointing to
	// _mProj: matrix to point to
	static void SetProjMat(CMath::TMATRIX& _mProj);

	// Returns the mouse x y z in world space
	static void GetMouseWorldSpace(const CMath::TVECTOR2 &vScreenSpacePosition, CMath::TVECTOR3 &vWorldStart, CMath::TVECTOR3 &vWorldEnd);

	// WARNING: THIS FUNCTION SHOULD ONLY BE DECLARED IN MAIN
	// CALLING THIS ANYWHERE ELSE CAN BREAK THE INPUT MANAGER
	// Processes window inputs
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// WARNING: THIS FUNCTION SHOULD ONLY BE DECLARED IN MAIN AND WNDPROC
	// CALLING THIS ANYWHERE ELSE CAN BREAK THE INPUT MANAGER
	// This function is utilized to allow keys to
	// return true in GetKeyDown again
	static void UpdatePresses();

	// Sets the Mouse Position to the center of the screen
	static void SetMouseToCenter();

	// Gets the direction the Mouse Wheel is being moved
	// 1: Scroll Up
	// -1: Scroll Down
	// 0: Not Scrolling
	static int GetScrollDirection();

	// Returns true if any button being tracked is being held
	static bool GetAnyKey();

	// Returns an unsigned int if any button tracked is being pressed
	// Returns -1 if none are
	static int GetAnyKeyDown();

};

