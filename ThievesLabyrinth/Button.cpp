#pragma once
#include "Button.h"
#include "InputManager.h"


CButton::CButton(void (*pFunction)(), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	m_pFunction = pFunction; // Function to run
	m_pButtonText = buttonText; // Text to render
	// Boundary stuff
	m_fLeft = left;
	m_fRight = right;
	m_fTop = top;
	m_fBottom = bottom;
	// Set the boundary for the button rendering and collision
	ChangeBoundary((float)width, (float)height, wWidth, wHeight);
	m_nTextLength = wcslen(buttonText); // This returns a size_t for some reason
}


bool CButton::IsHovered()
{
	return m_bHovered;
}

CButton::~CButton()
{
	m_pFunction = nullptr;
}

bool CButton::CheckMouseCollision() //Fix if statement
{
	int nMouseX = 0, nMouseY = 0;
	CInputManager::GetMousePos(nMouseX, nMouseY);
	// Check to see if the mouse is within our bounds
	if (nMouseX <= (m_Collision.right) && nMouseX >= (m_Collision.left)) {
		if (nMouseY <= (m_Collision.bottom) && nMouseY >= (m_Collision.top)) {
			return true;
		}
	}
	return false;
}

void CButton::Update()
{
	// Check if the mouse is inside the collision box
	if (CheckMouseCollision())
	{
		if (CInputManager::GetKeyDown(VK_LBUTTON))
		{
			// Function will be playing the sound
			// Should think about doing it here
			m_pFunction();
		}
		else
		{
			// TODO: Indicate the button is moused-over
			m_bHovered = true;
		}
	}
	else
	{
		m_bHovered = false;
	}
}

void CButton::ChangeBoundary(float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	// Set rendering box to new dimensions
	m_fBox.left = fWidth * m_fLeft;
	m_fBox.right = fWidth * m_fRight;
	m_fBox.top = fHeight * m_fTop;
	m_fBox.bottom = fHeight * m_fBottom;
	// Set collision box to new dimensions
	m_Collision.left = (long)(fwWidth * m_fLeft);
	m_Collision.right = (long)(fwWidth * m_fRight);
	m_Collision.top = (long)(fwHeight * m_fTop);
	m_Collision.bottom = (long)(fwHeight * m_fBottom);
}

D2D1_RECT_F CButton::GetButtonBox()
{
	return m_fBox;
}

const WCHAR* CButton::GetButtonText()
{
	return m_pButtonText;
}

size_t CButton::GetTextLength()
{
	return m_nTextLength;
}

void CButton::ChangeLeftRight(float fLeft, float fRight)
{
	// Changes the left and right parameters for the slider function
	m_fLeft = fLeft;
	m_fRight = fRight;
}
