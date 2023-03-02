#pragma once
#include "InputManager.h"
#include "EventManager.h"

#include "Transform.h"

#include <DirectXMath.h>
#include <windowsx.h>
#include <iostream>

std::bitset<NBITSIZE> CInputManager::m_bKeys;
std::bitset<NBITSIZE> CInputManager::m_bKeysDown(0xffffffffffffff);
std::bitset<NBITSIZE> CInputManager::m_bKeyBools(0xffffffffffffff);
int CInputManager::nMouseXPos = 0;
int CInputManager::nMouseYPos = 0;
int CInputManager::nScreenWidth = 0;
int CInputManager::nScreenHeight = 0;
int CInputManager::nXDiff = 0;
int CInputManager::nYDiff = 0;
const CMath::TMATRIX* CInputManager::m_pProjMat;
CTransform* CInputManager::m_pCameraTransform;
RECT CInputManager::m_WindowBox;
bool CInputManager::m_bUpdateScroll;
int CInputManager::m_nScrollDirection;

bool CInputManager::GetKey(char chKey)		
{
	switch (chKey)
	{
	case 'W':
	{
		if (m_bKeys[0] == 1)		//Every if statement requires brackets 
			return true;
		break;
	}
	case 'A':
	{
		if (m_bKeys[1] == 1)
			return true;
		break;
	}
	case 'S':
	{
		if (m_bKeys[2] == 1)
			return true;
		break;
	}
	case 'D':
	{
		if (m_bKeys[3] == 1)
			return true;
		break;
	}
	case VK_LBUTTON:
	{
		if (m_bKeys[4] == 1)
			return true;
		break;
	}
	case VK_UP:
	{
		if (m_bKeys[5] == 1)
			return true;
		break;
	}
	case VK_LEFT:
	{
		if (m_bKeys[6] == 1)
			return true;
		break;
	}
	case VK_DOWN:
	{
		if (m_bKeys[7] == 1)
			return true;
		break;
	}
	case VK_RIGHT:
	{
		if (m_bKeys[8] == 1)
			return true;
		break;
	}
	case 'B':
	{
		if (m_bKeys[9] == 1)
			return true;
		break;
	}
	case 'C':
	{
		if (m_bKeys[10] == 1)
			return true;
		break;
	}
	case 'E':
	{
		if (m_bKeys[11] == 1)
			return true;
		break;
	}
	case 'F':
	{
		if (m_bKeys[12] == 1)
			return true;
		break;
	}
	case 'G':
	{
		if (m_bKeys[13] == 1)
			return true;
		break;
	}
	case 'H':
	{
		if (m_bKeys[14] == 1)
			return true;
		break;
	}
	case 'I':
	{
		if (m_bKeys[15] == 1)
			return true;
		break;
	}
	case 'J':
	{
		if (m_bKeys[16] == 1)
			return true;
		break;
	}
	case 'K':
	{
		if (m_bKeys[17] == 1)
			return true;
		break;
	}
	case 'L':
	{
		if (m_bKeys[18] == 1)
			return true;
		break;
	}
	case 'M':
	{
		if (m_bKeys[19] == 1)
			return true;
		break;
	}
	case 'N':
	{
		if (m_bKeys[20] == 1)
			return true;
		break;
	}
	case 'O':
	{
		if (m_bKeys[21] == 1)
			return true;
		break;
	}
	case 'P':
	{
		if (m_bKeys[22] == 1)
			return true;
		break;
	}
	case 'Q':
	{
		if (m_bKeys[23] == 1)
			return true;
		break;
	}
	case 'R':
	{
		if (m_bKeys[24] == 1)
			return true;
		break;
	}
	case 'T':
	{
		if (m_bKeys[25] == 1)
			return true;
		break;
	}
	case 'U':
	{
		if (m_bKeys[26] == 1)
			return true;
		break;
	}
	case 'V':
	{
		if (m_bKeys[27] == 1)
			return true;
		break;
	}
	case 'X':
	{
		if (m_bKeys[28] == 1)
			return true;
		break;
	}
	case 'Y':
	{
		if (m_bKeys[29] == 1)
			return true;
		break;
	}
	case 'Z':
	{
		if (m_bKeys[30] == 1)
			return true;
		break;
	}
	case VK_ESCAPE:
	{
		if (m_bKeys[31] == 1)
			return true;
		break;
	}
	case VK_RBUTTON:
	{
		if (m_bKeys[32] == 1)
			return true;
		break;
	}
	case '1':
	{
		if (m_bKeys[33] == 1)
			return true;
		break;
	}
	case '2':
	{
		if (m_bKeys[34] == 1)
			return true;
		break;
	}
	case '3':
	{
		if (m_bKeys[35] == 1)
			return true;
		break;
	}
	case VK_SPACE:
	{
		if (m_bKeys[36] == 1)
			return true;
		break;
	}
	case VK_TAB:
	{
		if (m_bKeys[37] == 1)
			return true;
		break;
	}
	case VK_F1:
	{
		if (m_bKeys[38] == 1)
			return true;
		break;
	}
	case VK_F2:
	{
		if (m_bKeys[39] == 1)
			return true;
		break;
	}
	case VK_F3:
	{
		if (m_bKeys[40] == 1)
			return true;
		break;
	}
	case VK_F4:
	{
		if (m_bKeys[41] == 1)
			return true;
		break;
	}
	case VK_F5:
	{
		if (m_bKeys[42] == 1)
			return true;
		break;
	}
	case VK_F6:
	{
		if (m_bKeys[43] == 1)
			return true;
		break;
	}
	case VK_F7:
	{
		if (m_bKeys[44] == 1)
			return true;
		break;
	}
	case VK_F8:
	{
		if (m_bKeys[45] == 1)
			return true;
		break;
	}
	case VK_F9:
	{
		if (m_bKeys[46] == 1)
			return true;
		break;
	}
	case VK_PRIOR:
	{
		if (m_bKeys[47] == 1)
			return true;
		break;
	}
	case VK_F11:
	{
		if (m_bKeys[48] == 1)
			return true;
		break;
	}
	case VK_NEXT:
	{
		if (m_bKeys[49] == 1)
			return true;
		break;
	}
	case VK_CONTROL:
	{
		if (m_bKeys[50] == 1)
			return true;
		break;

	}
	case '4':
	{
		if (m_bKeys[51] == 1)
		return true;
		break;
	}
	case '5':
	{
		if (m_bKeys[52] == 1)
			return true;
		break;
	}
	default:
		break;
	}
	return false;
};

bool CInputManager::GetKeyDown(char chKey)
{
	switch (chKey)
	{
	case 'W':
	{
		if ((m_bKeys[0] == 1) && (m_bKeysDown[0] == 1))
		{
			m_bKeyBools[0] = 0;
			return true;
		}
		break;
	}
	case 'A':
	{
		if ((m_bKeys[1] == 1) && (m_bKeysDown[1] == 1))
		{
			m_bKeyBools[1] = 0;
			return true;
		}
		break;
	}
	case 'S':
	{
		if ((m_bKeys[2] == 1) && (m_bKeysDown[2] == 1))
		{
			m_bKeyBools[2] = 0;
			return true;
		}
		break;
	}
	case 'D':
	{
		if ((m_bKeys[3] == 1) && (m_bKeysDown[3] == 1))
		{
			m_bKeyBools[3] = 0;
			return true;
		}
		break;
	}
	case VK_LBUTTON:
	{
		if ((m_bKeys[4] == 1) && (m_bKeysDown[4] == 1))
		{
			m_bKeyBools[4] = 0;
			return true;
		}
		break;
	}
	case VK_UP:
	{
		if ((m_bKeys[5] == 1) && (m_bKeysDown[5] == 1))
		{
			m_bKeyBools[5] = 0;
			return true;
		}
		break;
	}
	case VK_LEFT:
	{
		if ((m_bKeys[6] == 1) && (m_bKeysDown[6] == 1))
		{
			m_bKeyBools[6] = 0;
			return true;
		}
		break;
	}
	case VK_DOWN:
	{
		if ((m_bKeys[7] == 1) && (m_bKeysDown[7] == 1))
		{
			m_bKeyBools[7] = 0;
			return true;
		}
		break;
	}
	case VK_RIGHT:
	{
		if ((m_bKeys[8] == 1) && (m_bKeysDown[8] == 1))
		{
			m_bKeyBools[8] = 0;
			return true;
		}
		break;
	}
	case 'B':
	{
		if ((m_bKeys[9] == 1) && (m_bKeysDown[9] == 1))
		{
			m_bKeyBools[9] = 0;
			return true;
		}
		break;
	}
	case 'C':
	{
		if ((m_bKeys[10] == 1) && (m_bKeysDown[10] == 1))
		{
			m_bKeyBools[10] = 0;
			return true;
		}
		break;
	}
	case 'E':
	{
		if ((m_bKeys[11] == 1) && (m_bKeysDown[11] == 1))
		{
			m_bKeyBools[11] = 0;
			return true;
		}
		break;
	}
	case 'F':
	{
		if ((m_bKeys[12] == 1) && (m_bKeysDown[12] == 1))
		{
			m_bKeyBools[12] = 0;
			return true;
		}
		break;
	}
	case 'G':
	{
		if ((m_bKeys[13] == 1) && (m_bKeysDown[13] == 1))
		{
			m_bKeyBools[13] = 0;
			return true;
		}
		break;
	}
	case 'H':
	{
		if ((m_bKeys[14] == 1) && (m_bKeysDown[14] == 1))
		{
			m_bKeyBools[14] = 0;
			return true;
		}
		break;
	}
	case 'I':
	{
		if ((m_bKeys[15] == 1) && (m_bKeysDown[15] == 1))
		{
			m_bKeyBools[15] = 0;
			return true;
		}
		break;
	}
	case 'J':
	{
		if ((m_bKeys[16] == 1) && (m_bKeysDown[16] == 1))
		{
			m_bKeyBools[16] = 0;
			return true;
		}
		break;
	}
	case 'K':
	{
		if ((m_bKeys[17] == 1) && (m_bKeysDown[17] == 1))
		{
			m_bKeyBools[17] = 0;
			return true;
		}
		break;
	}
	case 'L':
	{
		if ((m_bKeys[18] == 1) && (m_bKeysDown[18] == 1))
		{
			m_bKeyBools[18] = 0;
			return true;
		}
		break;
	}
	case 'M':
	{
		if ((m_bKeys[19] == 1) && (m_bKeysDown[19] == 1))
		{
			m_bKeyBools[19] = 0;
			return true;
		}
		break;
	}
	case 'N':
	{
		if ((m_bKeys[20] == 1) && (m_bKeysDown[20] == 1))
		{
			m_bKeyBools[20] = 0;
			return true;
		}
		break;
	}
	case 'O':
	{
		if ((m_bKeys[21] == 1) && (m_bKeysDown[21] == 1))
		{
			m_bKeyBools[21] = 0;
			return true;
		}
		break;
	}
	case 'P':
	{
		if ((m_bKeys[22] == 1) && (m_bKeysDown[22] == 1))
		{
			m_bKeyBools[22] = 0;
			return true;
		}
		break;
	}
	case 'Q':
	{
		if ((m_bKeys[23] == 1) && (m_bKeysDown[23] == 1))
		{
			m_bKeyBools[23] = 0;
			return true;
		}
		break;
	}
	case 'R':
	{
		if ((m_bKeys[24] == 1) && (m_bKeysDown[24] == 1))
		{
			m_bKeyBools[24] = 0;
			return true;
		}
		break;
	}
	case 'T':
	{
		if ((m_bKeys[25] == 1) && (m_bKeysDown[25] == 1))
		{
			m_bKeyBools[25] = 0;
			return true;
		}
		break;
	}
	case 'U':
	{
		if ((m_bKeys[26] == 1) && (m_bKeysDown[26] == 1))
		{
			m_bKeyBools[26] = 0;
			return true;
		}
		break;
	}
	case 'V':
	{
		if ((m_bKeys[27] == 1) && (m_bKeysDown[27] == 1))
		{
			m_bKeyBools[27] = 0;
			return true;
		}
		break;
	}
	case 'X':
	{
		if ((m_bKeys[28] == 1) && (m_bKeysDown[28] == 1))
		{
			m_bKeyBools[28] = 0;
			return true;
		}
		break;
	}
	case 'Y':
	{
		if ((m_bKeys[29] == 1) && (m_bKeysDown[29] == 1))
		{
			m_bKeyBools[29] = 0;
			return true;
		}
		break;
	}
	case 'Z':
	{
		if ((m_bKeys[30] == 1) && (m_bKeysDown[30] == 1))
		{
			m_bKeyBools[30] = 0;
			return true;
		}
		break;
	}
	case VK_ESCAPE:
	{
		if ((m_bKeys[31] == 1) && (m_bKeysDown[31] == 1))
		{
			m_bKeyBools[31] = 0;
			return true;
		}
		break;
	}
	case VK_RBUTTON:
	{
		if ((m_bKeys[32] == 1) && (m_bKeysDown[32] == 1))
		{
			m_bKeyBools[32] = 0;
			return true;
		}
		break;
	}
	case '1':
	{
		if ((m_bKeys[33] == 1) && (m_bKeysDown[33] == 1))
		{
			m_bKeyBools[33] = 0;
			return true;
		}
		break;
	}
	case '2':
	{
		if ((m_bKeys[34] == 1) && (m_bKeysDown[34] == 1))
		{
			m_bKeyBools[34] = 0;
			return true;
		}
		break;
	}
	case '3':
	{
		if ((m_bKeys[35] == 1) && (m_bKeysDown[35] == 1))
		{
			m_bKeyBools[35] = 0;
			return true;
		}
		break;
	}
	case VK_SPACE:
	{
		if ((m_bKeys[36] == 1) && (m_bKeysDown[36] == 1))
		{
			m_bKeyBools[36] = 0;
			return true;
		}
		break;
	}
	case VK_TAB:
	{
		if ((m_bKeys[37] == 1) && (m_bKeysDown[37] == 1))
		{
			m_bKeyBools[37] = 0;
			return true;
		}
		break;
	}
	case VK_F1:
	{
		if ((m_bKeys[38] == 1) && (m_bKeysDown[38] == 1))
		{
			m_bKeyBools[38] = 0;
			return true;
		}
		break;
	}
	case VK_F2:
	{
		if ((m_bKeys[39] == 1) && (m_bKeysDown[39] == 1))
		{
			m_bKeyBools[39] = 0;
			return true;
		}
		break;
	}
	case VK_F3:
	{
		if ((m_bKeys[40] == 1) && (m_bKeysDown[40] == 1))
		{
			m_bKeyBools[40] = 0;
			return true;
		}
		break;
	}
	case VK_F4:
	{
		if ((m_bKeys[41] == 1) && (m_bKeysDown[41] == 1))
		{
			m_bKeyBools[41] = 0;
			return true;
		}
		break;
	}
	case VK_F5:
	{
		if ((m_bKeys[42] == 1) && (m_bKeysDown[42] == 1))
		{
			m_bKeyBools[42] = 0;
			return true;
		}
		break;
	}
	case VK_F6:
	{
		if ((m_bKeys[43] == 1) && (m_bKeysDown[43] == 1))
		{
			m_bKeyBools[43] = 0;
			return true;
		}
		break;
	}
	case VK_F7:
	{
		if ((m_bKeys[44] == 1) && (m_bKeysDown[44] == 1))
		{
			m_bKeyBools[44] = 0;
			return true;
		}
		break;
	}
	case VK_F8:
	{
		if ((m_bKeys[45] == 1) && (m_bKeysDown[45] == 1))
		{
			m_bKeyBools[45] = 0;
			return true;
		}
		break;
	}
	case VK_F9:
	{
		if ((m_bKeys[46] == 1) && (m_bKeysDown[46] == 1))
		{
			m_bKeyBools[46] = 0;
			return true;
		}
		break;
	}
	case VK_PRIOR:
	{
		if ((m_bKeys[47] == 1) && (m_bKeysDown[47] == 1))
		{
			m_bKeyBools[47] = 0;
			return true;
		}
		break;
	}
	case VK_F11:
	{
		if ((m_bKeys[48] == 1) && (m_bKeysDown[48] == 1))
		{
			m_bKeyBools[48] = 0;
			return true;
		}
		break;
	}
	case VK_NEXT:
	{
		if ((m_bKeys[49] == 1) && (m_bKeysDown[49] == 1))
		{
			m_bKeyBools[49] = 0;
			return true;
		}
		break;
	}
	case VK_CONTROL:
	{
		if ((m_bKeys[50] == 1) && (m_bKeysDown[50] == 1))
		{
			m_bKeyBools[50] = 0;
			return true;
		}
		break;
	}
	case '4':
	{
		if ((m_bKeys[51] == 1) && (m_bKeysDown[51] == 1))
		{
			m_bKeyBools[51] = 0;
			return true;
		}
		break;
	}
	case '5':
	{
		if ((m_bKeys[52] == 1) && (m_bKeysDown[52] == 1))
		{
			m_bKeyBools[52] = 0;
			return true;
		}
		break;
	}
	default:
		break;
	}
	return false;
};

void CInputManager::UpdatePresses()
{
	for (int i = 0; i < NBITSIZE; i++)
	{
		m_bKeysDown[i] = m_bKeyBools[i];
	}
	if (m_bUpdateScroll)
	{
		m_nScrollDirection = 0;
	}
}

void CInputManager::SetMouseToCenter()
{
	SetCursorPos(m_WindowBox.left + ((m_WindowBox.right - m_WindowBox.left) / 2), (m_WindowBox.top + ((m_WindowBox.bottom - m_WindowBox.top) / 2)));
}

int CInputManager::GetScrollDirection()
{
	m_bUpdateScroll = true;
	return m_nScrollDirection;
}

bool CInputManager::GetAnyKey()
{
	for (int i = 0; i < NBITSIZE; i++)
	{
		if (m_bKeys[i] == 1)
		{
			return true;
		}
	}
	return false;
}

int CInputManager::GetAnyKeyDown()
{
	for (int i = 0; i < NBITSIZE; i++)
	{
		if (m_bKeys[i] == 1 && m_bKeysDown[i] == 1)
		{
			for (int j = 0; j < NBITSIZE; j++)
			{
				m_bKeyBools[i] = 0;
			}
			return i;
		}
	}
	return -1;
}

void CInputManager::GetMousePos(int& nMouseX, int& nMouseY)
{
	nMouseX = nMouseXPos;
	nMouseY = nMouseYPos;
}

void CInputManager::GetMousePosNDC(float & _nX, float & _nY)
{
	_nX = (nMouseXPos / (nScreenWidth / 2.0f)) - 1;
	_nY = (-nMouseYPos / (nScreenHeight / 2.0f)) + 1;
}

void CInputManager::GetScreenWH(int & nWidth, int & nHeight)
{
	nWidth = nScreenWidth;
	nHeight = nScreenHeight;
}

void CInputManager::SetViewTransform(CTransform & _mView)
{
	m_pCameraTransform = &_mView;
}

void CInputManager::SetProjMat(CMath::TMATRIX & _mProj)
{
	m_pProjMat = &_mProj;
}

void CInputManager::GetMouseWorldSpace(const CMath::TVECTOR2 & vScreenSpacePosition, CMath::TVECTOR3 & vWorldStart, CMath::TVECTOR3 & vWorldEnd)
{
	CMath::TVECTOR4 vScreenStart(vScreenSpacePosition.x, vScreenSpacePosition.y, 0.0f, 1.0f);
	CMath::TVECTOR4 vScreenEnd	(vScreenSpacePosition.x, vScreenSpacePosition.y, 1.0f, 1.0f);
	
	CMath::TMATRIX mProjectionInverse, mViewProjection;
	mProjectionInverse = CMath::MatrixInverse(*m_pProjMat);
	mViewProjection = mProjectionInverse * m_pCameraTransform->GetMatrix();

	CMath::TVECTOR4 vWorldStart4, vWorldEnd4;
	vWorldStart4 = CMath::Vector4Transform(vScreenStart, mViewProjection);
	vWorldEnd4	 = CMath::Vector4Transform(vScreenEnd, mViewProjection);

	vWorldStart	= *(CMath::TVECTOR3*)&vWorldStart4;
	vWorldEnd	= *(CMath::TVECTOR3*)&vWorldEnd4;
	vWorldStart	/= vWorldStart4.w;
	vWorldEnd	/= vWorldEnd4.w;
}

LRESULT CALLBACK CInputManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bSkipDefWinProc = false;

	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	case WM_SYSCOMMAND:
	{
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			bSkipDefWinProc = true;
		break;
	}

	case WM_KEYDOWN:
	{
		if (wParam == 'W')
			m_bKeys[0] = 1;
		if (wParam == 'A')
			m_bKeys[1] = 1;
		if (wParam == 'S')
			m_bKeys[2] = 1;
		if (wParam == 'D')
			m_bKeys[3] = 1;
		if (wParam == VK_UP)
			m_bKeys[5] = 1;
		if (wParam == VK_LEFT)
			m_bKeys[6] = 1;
		if (wParam == VK_DOWN)
			m_bKeys[7] = 1;
		if (wParam == VK_RIGHT)
			m_bKeys[8] = 1;
		if (wParam == 'B')
			m_bKeys[9] = 1;
		if (wParam == 'C')
			m_bKeys[10] = 1;
		if (wParam == 'E')
			m_bKeys[11] = 1;
		if (wParam == 'F')
			m_bKeys[12] = 1;
		if (wParam == 'G')
			m_bKeys[13] = 1;
		if (wParam == 'H')
			m_bKeys[14] = 1;
		if (wParam == 'I')
			m_bKeys[15] = 1;
		if (wParam == 'J')
			m_bKeys[16] = 1;
		if (wParam == 'K')
			m_bKeys[17] = 1;
		if (wParam == 'L')
			m_bKeys[18] = 1;
		if (wParam == 'M')
			m_bKeys[19] = 1;
		if (wParam == 'N')
			m_bKeys[20] = 1;
		if (wParam == 'O')
			m_bKeys[21] = 1;
		if (wParam == 'P')
			m_bKeys[22] = 1;
		if (wParam == 'Q')
			m_bKeys[23] = 1;
		if (wParam == 'R')
			m_bKeys[24] = 1;
		if (wParam == 'T')
			m_bKeys[25] = 1;
		if (wParam == 'U')
			m_bKeys[26] = 1;
		if (wParam == 'V')
			m_bKeys[27] = 1;
		if (wParam == 'X')
			m_bKeys[28] = 1;
		if (wParam == 'Y')
			m_bKeys[29] = 1;
		if (wParam == 'Z')
			m_bKeys[30] = 1;
		if (wParam == VK_ESCAPE)
			m_bKeys[31] = 1;
		if (wParam == '1')
			m_bKeys[33] = 1;
		if (wParam == '2')
			m_bKeys[34] = 1;
		if (wParam == '3')
			m_bKeys[35] = 1;
		if (wParam == VK_SPACE)
			m_bKeys[36] = 1;
		if (wParam == VK_TAB)
			m_bKeys[37] = 1;
		if (wParam == VK_F1)
			m_bKeys[38] = 1;
		if (wParam == VK_F2)
			m_bKeys[39] = 1;
		if (wParam == VK_F3)
			m_bKeys[40] = 1;
		if (wParam == VK_F4)
			m_bKeys[41] = 1;
		if (wParam == VK_F5)
			m_bKeys[42] = 1;
		if (wParam == VK_F6)
			m_bKeys[43] = 1;
		if (wParam == VK_F7)
			m_bKeys[44] = 1;
		if (wParam == VK_F8)
			m_bKeys[45] = 1;
		if (wParam == VK_F9)
			m_bKeys[46] = 1;
		if (wParam == VK_PRIOR)
			m_bKeys[47] = 1;
		if (wParam == VK_F11)
			m_bKeys[48] = 1;
		if (wParam == VK_NEXT)
			m_bKeys[49] = 1;
		if (wParam == VK_CONTROL)
			m_bKeys[50] = 1;
		if (wParam == '4')
			m_bKeys[51] = 1;
		if (wParam == '5')
			m_bKeys[52] = 1;
		break;
	}
	case WM_KEYUP:
	{
		if (wParam == 'W')
		{
			m_bKeys[0] = 0;
			m_bKeyBools[0] = 1;
		}
		if (wParam == 'A')
		{
			m_bKeys[1] = 0;
			m_bKeyBools[1] = 1;
		}		
		if (wParam == 'S')
		{
			m_bKeys[2] = 0;
			m_bKeyBools[2] = 1;
		}
		if (wParam == 'D')
		{
			m_bKeys[3] = 0;
			m_bKeyBools[3] = 1;
		}
		if (wParam == VK_UP)
		{
			m_bKeys[5] = 0;
			m_bKeyBools[5] = 1;
		}
		if (wParam == VK_LEFT)
		{
			m_bKeys[6] = 0;
			m_bKeyBools[6] = 1;
		}
		if (wParam == VK_DOWN)
		{
			m_bKeys[7] = 0;
			m_bKeyBools[7] = 1;
		}
		if (wParam == VK_RIGHT)
		{
			m_bKeys[8] = 0;
			m_bKeyBools[8] = 1;
		}
		if (wParam == 'B')
		{
			m_bKeys[9] = 0;
			m_bKeyBools[9] = 1;
		}
		if (wParam == 'C')
		{
			m_bKeys[10] = 0;
			m_bKeyBools[10] = 1;
		}
		if (wParam == 'E')
		{
			m_bKeys[11] = 0;
			m_bKeyBools[11] = 1;
		}
		if (wParam == 'F')
		{
			m_bKeys[12] = 0;
			m_bKeyBools[12] = 1;
		}
		if (wParam == 'G')
		{
			m_bKeys[13] = 0;
			m_bKeyBools[13] = 1;
		}
		if (wParam == 'H')
		{
			m_bKeys[14] = 0;
			m_bKeyBools[14] = 1;
		}
		if (wParam == 'I')
		{
			m_bKeys[15] = 0;
			m_bKeyBools[15] = 1;
		}
		if (wParam == 'J')
		{
			m_bKeys[16] = 0;
			m_bKeyBools[16] = 1;
		}
		if (wParam == 'K')
		{
			m_bKeys[17] = 0;
			m_bKeyBools[17] = 1;
		}
		if (wParam == 'L')
		{
			m_bKeys[18] = 0;
			m_bKeyBools[18] = 1;
		}
		if (wParam == 'M')
		{
			m_bKeys[19] = 0;
			m_bKeyBools[19] = 1;
		}
		if (wParam == 'N')
		{
			m_bKeys[20] = 0;
			m_bKeyBools[20] = 1;
		}		
		if (wParam == 'O')
		{
			m_bKeys[21] = 0;
			m_bKeyBools[21] = 1;
		}
		if (wParam == 'P')
		{
			m_bKeys[22] = 0;
			m_bKeyBools[22] = 1;
		}
		if (wParam == 'Q')
		{
			m_bKeys[23] = 0;
			m_bKeyBools[23] = 1;
		}
		if (wParam == 'R')
		{
			m_bKeys[24] = 0;
			m_bKeyBools[24] = 1;
		}
		if (wParam == 'T')
		{
			m_bKeys[25] = 0;
			m_bKeyBools[25] = 1;
		}
		if (wParam == 'U')
		{
			m_bKeys[26] = 0;
			m_bKeyBools[26] = 1;
		}
		if (wParam == 'V')
		{
			m_bKeys[27] = 0;
			m_bKeyBools[27] = 1;
		}
		if (wParam == 'X')
		{
			m_bKeys[28] = 0;
			m_bKeyBools[28] = 1;
		}
		if (wParam == 'Y')
		{
			m_bKeys[29] = 0;
			m_bKeyBools[29] = 1;
		}
		if (wParam == 'Z')
		{
			m_bKeys[30] = 0;
			m_bKeyBools[30] = 1;
		}
		if (wParam == VK_ESCAPE)
		{
			m_bKeys[31] = 0;
			m_bKeyBools[31] = 1;
		}
		if (wParam == '1')
		{
			m_bKeys[33] = 0;
			m_bKeyBools[33] = 1;
		}
		if (wParam == '2')
		{
			m_bKeys[34] = 0;
			m_bKeyBools[34] = 1;
		}
		if (wParam == '3')
		{
			m_bKeys[35] = 0;
			m_bKeyBools[35] = 1;
		}
		if (wParam == VK_SPACE)
		{
			m_bKeys[36] = 0;
			m_bKeyBools[36] = 1;
		}
		if (wParam == VK_TAB)
		{
			m_bKeys[37] = 0;
			m_bKeyBools[37] = 1;
		}
		if (wParam == VK_F1)
		{
			m_bKeys[38] = 0;
			m_bKeyBools[38] = 1;
		}
		if (wParam == VK_F2)
		{
			m_bKeys[39] = 0;
			m_bKeyBools[39] = 1;
		}
		if (wParam == VK_F3)
		{
			m_bKeys[40] = 0;
			m_bKeyBools[40] = 1;
		}
		if (wParam == VK_F4)
		{
			m_bKeys[41] = 0;
			m_bKeyBools[41] = 1;
		}
		if (wParam == VK_F5)
		{
			m_bKeys[42] = 0;
			m_bKeyBools[42] = 1;
		}
		if (wParam == VK_F6)
		{
			m_bKeys[43] = 0;
			m_bKeyBools[43] = 1;
		}
		if (wParam == VK_F7)
		{
			m_bKeys[44] = 0;
			m_bKeyBools[44] = 1;
		}
		if (wParam == VK_F8)
		{
			m_bKeys[45] = 0;
			m_bKeyBools[45] = 1;
		}
		if (wParam == VK_F9)
		{
			m_bKeys[46] = 0;
			m_bKeyBools[46] = 1;
		}
		if (wParam == VK_PRIOR)
		{
			m_bKeys[47] = 0;
			m_bKeyBools[47] = 1;
		}
		if (wParam == VK_F11)
		{
			m_bKeys[48] = 0;
			m_bKeyBools[48] = 1;
		}
		if (wParam == VK_NEXT)
		{
			m_bKeys[49] = 0;
			m_bKeyBools[49] = 1;
		}
		if (wParam == VK_CONTROL)
		{
			m_bKeys[50] = 0;
			m_bKeyBools[50] = 1;
		}
		if (wParam == '4')
		{
			m_bKeys[51] = 0;
			m_bKeyBools[51] = 1;
		}
		if (wParam == '5')
		{
			m_bKeys[52] = 0;
			m_bKeyBools[52] = 1;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		m_bKeys[4] = 1;
		break;
	}
	case WM_LBUTTONUP:
	{
		m_bKeys[4] = 0;
		m_bKeyBools[4] = 1;
		break;
	}
	case WM_RBUTTONDOWN:
	{
		m_bKeys[32] = 1;
		break;
	}
	case WM_RBUTTONUP:
	{
		m_bKeys[32] = 0;
		m_bKeyBools[32] = 1;
		break;
	}
	case WM_MOUSEMOVE:
	{
		nXDiff = nMouseXPos;
		nYDiff = nMouseYPos;
		nMouseXPos = GET_X_LPARAM(lParam);
		nMouseYPos = GET_Y_LPARAM(lParam);
		if (!m_bKeys[32])
		{
			nXDiff = nMouseXPos;
			nYDiff = nMouseYPos;
		}
		break;
	}
	case WM_SIZE:
	{
		nScreenWidth = LOWORD(lParam);
		nScreenHeight = HIWORD(lParam);
		// Call function to event manager
		// To resize all the stuff
		if (nScreenHeight != 0 && nScreenWidth != 0)
			CEventManager::SendResizeMessage(TWindowResizeMessage(nScreenWidth, nScreenHeight));
		break;
	}
	case WM_MOUSEWHEEL:
	{
		short direction = HIWORD(wParam);
		if (direction > 0)
		{
			m_nScrollDirection = 1;
		}
		else if (direction < 0)
		{
			m_nScrollDirection = -1;
		}
		else
		{
			m_nScrollDirection = 0;
		}
		m_bUpdateScroll = false;
		break;
	}
	default:
		break;
	}
	CInputManager::UpdatePresses();
	GetWindowRect(hWnd, &m_WindowBox);
	return bSkipDefWinProc ? 0 : DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CInputManager::GetMouseDelta(int& nDiffX, int& nDiffY)
{
	nDiffX = nMouseXPos - nXDiff;
	nDiffY = nMouseYPos - nYDiff;

	nXDiff = nMouseXPos;
	nYDiff = nMouseYPos;
}