#pragma once
#include "Slider.h"
#include "EventManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "EnumTypes.h"
#include "RenderManager.h"

bool CSlider::m_bSMaC, CSlider::m_bSMuC, CSlider::m_bSSC, CSlider::m_bPMaC, CSlider::m_bPMuC, CSlider::m_bPSC, CSlider::m_bSG, CSlider::m_bPG;

CSlider::CSlider(int menu, int nVary, int nType, float fWidth, float fHeight, float fwWidth, float fwHeight,
	float fLeft, float fRight, float fTop, float fBottom)
{
	m_pcButtonToMove = nullptr;
	// Static variables must be set to false
	m_bSMaC = m_bSMuC = m_bSSC = m_bPMaC = m_bPMuC = m_bPSC = false;
	// Hold onto swapchain and window width and height
	m_fWidth = fWidth;		m_fHeight = fHeight;
	m_fwWidth = fwWidth;	m_fwHeight = fwHeight;
	// Hold onto these for resize events
	m_fLeft = fLeft;		m_fRight = fRight;
	m_fTop = fTop;			m_fBottom = fBottom;
	m_nType = nType;		m_nMenu = menu;
	// The reason why we hold onto both window dimensions
	// Ensures the button remains within the bar
	m_fBoundsMax = m_fwWidth * m_fRight;
	m_fBoundsMin = m_fwWidth * m_fLeft;
	// Sets the value the slider directly influences
	m_nVary = nVary;
	if (nVary == 0)
	{
		switch (m_nType)
		{
		case eAudio::MUSIC:
		{
			m_fValue = CAudioManager::GetMusicVol();
			break;
		}
		case eAudio::SFX:
		{
			m_fValue = CAudioManager::GetSFXVol();
			break;
		}
		case eAudio::COUNT:
		{
			m_fValue = CAudioManager::GetMasterVol();
			break;
		}
		default:
			m_fValue = 0.0f;
			break;
		}
		// Figures out which menu we go to, and sets the button up for our specific needs
		switch (m_nMenu)
		{
		case eMenu::START_OPTIONS:
		{
			switch (m_nType)
			{
			case eAudio::MUSIC:
			{
				m_pcButtonToMove = new CButton(SMuC, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
					fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
				break;
			}
			case eAudio::SFX:
			{
				m_pcButtonToMove = new CButton(SSC, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
					fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
				break;
			}
			case eAudio::COUNT:
			{
				m_pcButtonToMove = new CButton(SMaC, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
					fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
				break;
			}
			default:
				break;
			}
			break;
		}
		case eMenu::PAUSE_OPTIONS:
		{
			switch (m_nType)
			{
			case eAudio::MUSIC:
			{
				m_pcButtonToMove = new CButton(PMuC, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
					fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
				break;
			}
			case eAudio::SFX:
			{
				m_pcButtonToMove = new CButton(PSC, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
					fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
				break;
			}
			case eAudio::COUNT:
			{
				m_pcButtonToMove = new CButton(PMaC, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
					fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
	}
	else
	{
		m_fValue = CRenderManager::GetBrightness();
		switch (m_nMenu)
		{
		case eMenu::START_OPTIONS:
		{
			m_pcButtonToMove = new CButton(SG, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
				fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
			break;
		}
		case eMenu::PAUSE_OPTIONS:
		{
			m_pcButtonToMove = new CButton(PG, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight,
				fLeft, fRight, fTop - 0.01f, fBottom + 0.01f);
			break;
		}
		default:
			break;
		}
	}
	

	// Sets the button to correct dimensions
	ChangePosition();
	// Slider Bar render box
	m_rBox = D2D1::RectF(fWidth * fLeft, fHeight * fTop, fWidth * fRight, fHeight * fBottom);
}

void CSlider::Update()
{
	if (!m_bSMaC && !m_bSMuC && !m_bSSC && !m_bPMaC && !m_bPMuC && !m_bPSC && !m_bSG && !m_bPG)
	{
		// Runs if all variables are false
		// Update value if another menu changed it
		if (m_nVary == 0)
		{

			switch (m_nType)
			{
			case eAudio::MUSIC:
			{
				m_fValue = CAudioManager::GetMusicVol();
				break;
			}
			case eAudio::SFX:
			{
				m_fValue = CAudioManager::GetSFXVol();
				break;
			}
			case eAudio::COUNT:
			{
				m_fValue = CAudioManager::GetMasterVol();
				break;
			}
			default:
				m_fValue = 0.0f;
				break;
			}
		}
		else
		{
			m_fValue = CRenderManager::GetBrightness();
		}
		// Update position for safety
		ChangePosition();
		// Run the update function on the button
		m_pcButtonToMove->Update();
	}
	else if (CInputManager::GetKey(VK_LBUTTON))
	{
		// One of the booleans are true, run this if the left mouse button is being held down
		bool change;
		// Check to see what slider needs to change
		if (m_nVary == 0)
		{
			switch (m_nMenu)
			{
			case eMenu::START_OPTIONS:
			{
				switch (m_nType)
				{
				case eAudio::MUSIC:
				{
					change = m_bSMuC;
					break;
				}
				case eAudio::SFX:
				{
					change = m_bSSC;
					break;
				}
				case eAudio::COUNT:
				{
					change = m_bSMaC;
					break;
				}
				default:
					change = false;
					break;
				}
				break;
			}
			case eMenu::PAUSE_OPTIONS:
			{
				switch (m_nType)
				{
				case eAudio::MUSIC:
				{
					change = m_bPMuC;
					break;
				}
				case eAudio::SFX:
				{
					change = m_bPSC;
					break;
				}
				case eAudio::COUNT:
				{
					change = m_bPMaC;
					break;
				}
				default:
					change = false;
					break;
				}
				break;
			}
			default:
				break;
			}
		}
		else
		{
			switch (m_nMenu)
			{
			case eMenu::START_OPTIONS:
			{
				change = m_bSG;
				break;
			}
			case eMenu::PAUSE_OPTIONS:
			{
				change = m_bPG;
				break;
			}
			default:
				change = false;
				break;
			}
		}
		if (change)
		{
			// Run this!  We are being changed!!
			ChangeSliderValue();
		}
	}
	else
	{
		if (m_bSSC || m_bPSC)
		{
			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::BUTTON));
		}
		// Set everything back to false, one of them is true and we aren't changing them
		m_bSMaC = m_bSMuC = m_bSSC = m_bPMaC = m_bPMuC = m_bPSC = m_bSG = m_bPG = false;
	}
}

float CSlider::GetValue()
{
	return m_fValue;
}

CSlider::~CSlider()
{
	if (m_pcButtonToMove) delete m_pcButtonToMove;
}


void CSlider::ChangeSliderValue()
{
	int mouseX = 0, mouseY = 0;
	CInputManager::GetMousePos(mouseX, mouseY);
	// If the mouse is outside the bounds, default value to 1 or 0
	if (mouseX >= m_fBoundsMax)
	{
		m_fValue = 1.0f;
	}
	else if (mouseX <= m_fBoundsMin)
	{
		m_fValue = 0.0f;
	}
	else
	{
		// Math to calculate the new value
		m_fValue = ((float)mouseX - m_fBoundsMin) / (m_fBoundsMax - m_fBoundsMin);
		ChangePosition();
	}

	// Change the volume depending on what we are changing
	if (m_nVary == 0)
	{
		switch (m_nType)
		{
		case eAudio::MUSIC:
		{
			CAudioManager::ChangeMusicVol(m_fValue);
			break;
		}
		case eAudio::SFX:
		{
			CAudioManager::ChangeSFXVol(m_fValue);
			break;
		}
		case eAudio::COUNT:
		{
			CAudioManager::ChangeMasterVol(m_fValue);
			break;
		}
		default:
			break;
		}
	}
	else
	{
		CRenderManager::SetBrightness(m_fValue);
	}
}

void CSlider::ResizeEvent(float width, float height, float wWidth, float wHeight)
{
	// Hold onto new window Widths and Heights
	m_fHeight = height; m_fWidth = width;
	m_fwHeight = wHeight; m_fwWidth = wWidth;
	// Change the bounds to lerp between 
	m_fBoundsMax = m_fwWidth * m_fRight;
	m_fBoundsMin = m_fwWidth * m_fLeft;
	// Update button position, just to be safe
	ChangePosition();
	// Update slider bar
	m_rBox = D2D1::RectF(width * m_fLeft, height * m_fTop, width * m_fRight, height * m_fBottom);
}

D2D1_RECT_F CSlider::GetBox()
{
	return m_rBox;
}

void CSlider::ChangePosition()
{
	// Get the center of the button in screen space
	float buttonCenter = m_fBoundsMin + ((m_fBoundsMax - m_fBoundsMin) * m_fValue);
	// Figure out where it would be with the swapchain width in mind
	float valueToChange = buttonCenter / m_fwWidth;
	// Change the button to render in a new spot
	m_pcButtonToMove->ChangeLeftRight(valueToChange - 0.02f, valueToChange + 0.02f);
	m_pcButtonToMove->ChangeBoundary(m_fWidth, m_fHeight, m_fwWidth, m_fwHeight);
}

void CSlider::SMaC()
{
	m_bSMaC = true;
}

void CSlider::SMuC()
{
	m_bSMuC = true;
}

void CSlider::SSC()
{
	m_bSSC = true;
}

void CSlider::PMaC()
{
	m_bPMaC = true;
}

void CSlider::PMuC()
{
	m_bPMuC = true;
}

void CSlider::PSC()
{
	m_bPSC = true;
}

void CSlider::SG()
{
	m_bSG = true;
}

void CSlider::PG()
{
	m_bPG = true;
}
