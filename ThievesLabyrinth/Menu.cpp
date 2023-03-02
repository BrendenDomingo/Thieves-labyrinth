#pragma once
#include "Menu.h"
#include "WICTextureLoader.h"

CMenu::CMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, bool isOptions)
{
	m_pd2dButtonSprite = nullptr;
	m_pd2dSpriteBatch = nullptr;
	m_pTitleText = nullptr;
	m_isOptions = isOptions;
	m_fLeft = m_fRight = m_fTop = m_fBottom = 0.0f;
	// Create the sprite batch to store button stuff to
	pd2dContext->CreateSpriteBatch(&m_pd2dSpriteBatch);
	ID3D11Texture2D* fromDisk = nullptr;
	ID3D11ShaderResourceView* ignored = nullptr;
	// Load button texture
	if (FAILED(DirectX::CreateWICTextureFromFile(pd3dDevice, L"../Assets/Textures/UI/ButtonSprites.png", (ID3D11Resource**)&fromDisk, &ignored)))
	{
		MessageBox(nullptr, L"Failed to load texture for buttons", L"Error", MB_OK);
		if (fromDisk)	fromDisk->Release();
		if (ignored)	ignored->Release();
		exit(0);
	}
	IDXGISurface2* texSurface = nullptr;
	fromDisk->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface);

	// Now to create a bitmap that will hold the button sprite
	DXGI_SURFACE_DESC sDesc;
	D2D1_BITMAP_PROPERTIES1 bitProp;
	ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
	ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
	texSurface->GetDesc(&sDesc);
	bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
	bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
	bitProp.dpiX = bitProp.dpiY = 96.0f; // THIS MUST NOT BE CHANGED!!
	// I don't know why, but not using this specific float can easily break EVERYTHING
	// If you put zero (which *some* functions seem to say is a-ok!) it will draw nothing!!!
	// You also get no errors, I guess it just assumes you are fine with drawing nothing at all.
	pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dButtonSprite);
	texSurface->Release();
	if (fromDisk) { fromDisk->Release(); fromDisk = nullptr; }
	if (ignored)  { ignored->Release();	 ignored = nullptr;	}

	// Because I'm crazy about making sure things are safe
	m_TitleBox = D2D1::RectF(0, 0, 0, 0);
	m_TitleLength = 0;

	m_UnpressedBoxes.push_back(D2D1::RectU(0, 0, 199, 199));
	m_UnpressedBoxes.push_back(D2D1::RectU(200, 0, 399, 199));
	m_UnpressedBoxes.push_back(D2D1::RectU(400, 0, 599, 199));
	m_UnpressedBoxes.push_back(D2D1::RectU(600, 0, 799, 199));
	m_PressedBoxes.push_back(D2D1::RectU(0, 200, 199, 399));
	m_PressedBoxes.push_back(D2D1::RectU(200, 200, 399, 399));
	m_PressedBoxes.push_back(D2D1::RectU(400, 200, 599, 399));
	m_PressedBoxes.push_back(D2D1::RectU(600, 200, 799, 399));

	if (isOptions)
	{
		m_pDescriptionText.push_back(L"Mouse Controls");
		m_pDescriptionText.push_back(L"Move: Left Mouse Button");
		m_pDescriptionText.push_back(L"Attack: Right Mouse Button");
		m_pDescriptionText.push_back(L"Swap Items: 1 2 3 4 5");
		m_pDescriptionText.push_back(L"Use Potion/Swap Passive Item: F Z X");
		m_pDescriptionText.push_back(L"Abilities: Q W E");
		m_pDescriptionText.push_back(L"Inventory: Tab");
		m_pDescriptionText.push_back(L"Keyboard Controls");
		m_pDescriptionText.push_back(L"Move: W A S D");
		m_pDescriptionText.push_back(L"Attack: Spacebar");
		m_pDescriptionText.push_back(L"Swap Items: Q E");
		m_pDescriptionText.push_back(L"Use Potion/Swap Passive Item: F Z X");
		m_pDescriptionText.push_back(L"Abilities: 1 2 3");
		m_pDescriptionText.push_back(L"Inventory: Tab");

		for (size_t i = 0; i < m_pDescriptionText.size(); i++)
		{
			m_DescriptionLength.push_back(wcslen(m_pDescriptionText[i]));
		}
		m_BoundaryBoxSet.push_back({ 0.3f, 0.25f, 0.7f, 0.35f });
		m_BoundaryBoxSet.push_back({ 0.2f, 0.35f, 0.45f, 0.45f });
		m_BoundaryBoxSet.push_back({ 0.55f, 0.35f, 0.8f, 0.45f });
		m_BoundaryBoxSet.push_back({ 0.2f, 0.5f, 0.45f, 0.6f });
		m_BoundaryBoxSet.push_back({ 0.55f, 0.5f, 0.8f, 0.6f });
		m_BoundaryBoxSet.push_back({ 0.2f, 0.65f, 0.45f, 0.75f });
		m_BoundaryBoxSet.push_back({ 0.55f, 0.65f, 0.8f, 0.75f });
		for (size_t i = 0; i < 7; i++)
		{
			m_DescriptionBoxes.push_back({ 0 });
		}
	}

}

bool CMenu::IsOptionsMenu()
{
	return m_isOptions;
}

void CMenu::ResizeBoxes(float fWidth, float fHeight)
{
	for (size_t i = 0; i < m_DescriptionBoxes.size(); i++)
	{
		float left = m_BoundaryBoxSet[i].left, right = m_BoundaryBoxSet[i].right,
			top = m_BoundaryBoxSet[i].top, bottom = m_BoundaryBoxSet[i].bottom;
		m_DescriptionBoxes[i] = D2D1::RectF(fWidth * left, fHeight * top, fWidth * right, fHeight * bottom);
	}
}

CMenu::~CMenu()
{
	for (size_t i = 0; i < m_pcButtons.size(); i++)
	{
		if (m_pcButtons[i])
			delete m_pcButtons[i];
	}
	for (size_t i = 0; i < m_pcSliders.size(); i++)
	{
		if (m_pcSliders[i])
			delete m_pcSliders[i];
	}
	if (m_pd2dButtonSprite) { m_pd2dButtonSprite->Release();	m_pd2dButtonSprite = nullptr; }
	if (m_pd2dSpriteBatch)	{ m_pd2dSpriteBatch->Release();		m_pd2dSpriteBatch = nullptr; }
}

void CMenu::AddButton(void(*pFunction)(void), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	CButton* pButton = new CButton(pFunction, buttonText, width, height, wWidth, wHeight, left, right, top, bottom);
	m_pcButtons.push_back(pButton);
	// This bit is to specify to the sprite batch to draw more stuff
	// The offscreen portion is a filler for what is required
	D2D1_RECT_F offscreen = { 0, };
	m_pd2dSpriteBatch->AddSprites(1, &offscreen);
}

void CMenu::AddSlider(int menu, int nVary, int nType, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	CSlider* pSlider = new CSlider(menu, nVary, nType, (float)width, (float)height, wWidth, wHeight, left, right, top, bottom);
	m_pcSliders.push_back(pSlider);
	// This bit is to specify to the sprite batch to draw more stuff
	// The offscreen portion is a filler for what is required
	D2D1_RECT_F offscreen[2] = { 0, };
	m_pd2dSpriteBatch->AddSprites(2, offscreen);
}

void CMenu::MakeTitle(const WCHAR* title_text, int width, int height, float left, float right, float top, float bottom)
{
	m_pTitleText = title_text;
	// Box to render title inside of
	m_TitleBox = D2D1::RectF(width * left, height * top, width * right, height * bottom);
	m_TitleLength = wcslen(title_text); // This function returns a size_t for some reason
	// Hold onto these for resize events
	m_fLeft = left; m_fRight = right; m_fTop = top; m_fBottom = bottom;
}

void CMenu::Update()
{
	for (size_t i = 0; i < m_pcButtons.size(); i++)
	{
		m_pcButtons[i]->Update();
	}
	for (size_t i = 0; i < m_pcSliders.size(); i++)
	{
		m_pcSliders[i]->Update();
	}
}

const WCHAR* CMenu::GetTitle()
{
	return m_pTitleText;
}

void CMenu::ResizeItems(float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	// Change the button positions and collisions
	for (size_t i = 0; i < m_pcButtons.size(); i++)
	{
		m_pcButtons[i]->ChangeBoundary(fWidth, fHeight, fwWidth, fwHeight);
	}
	// Change the slider positions and collisions
	for (size_t i = 0; i < m_pcSliders.size(); i++)
	{
		m_pcSliders[i]->ResizeEvent(fWidth, fHeight, fwWidth, fwHeight);
	}
	// Change the description text position
	ResizeBoxes(fWidth, fHeight);
	// Change the position of the Title Text
	m_TitleBox = D2D1::RectF(fWidth * m_fLeft, fHeight * m_fTop, fWidth * m_fRight, fHeight * m_fBottom);
}

void CMenu::AddDescriptionText(const WCHAR * text, int width, int height, float left, float right, float top, float bottom)
{
	m_pDescriptionText.push_back(text);
	// Store for resize events
	m_BoundaryBoxSet.push_back(D2D1::RectF(left, top, right, bottom));
	// Box to draw description text inside of
	m_DescriptionBoxes.push_back(D2D1::RectF(width * left, height * top, width * right, height * bottom));
	m_DescriptionLength.push_back(wcslen(text)); // This returns a size_t for some reason
}

size_t CMenu::GetTitleLength()
{
	return m_TitleLength;
}

D2D1_RECT_F CMenu::GetTitleBox()
{
	return m_TitleBox;
}

void CMenu::SetSprites()
{
	std::vector<D2D1_RECT_F> rectangles;
	std::vector<D2D1_RECT_U> sprites;
	// Take button positions and store them in the vector
	for (size_t i = 0; i < m_pcButtons.size(); i++)
	{
		rectangles.push_back(m_pcButtons[i]->GetButtonBox());
		if (m_pcButtons[i]->IsHovered())
		{
			sprites.push_back(m_PressedBoxes[i % m_PressedBoxes.size()]);
		}
		else
		{
			sprites.push_back(m_UnpressedBoxes[i % m_UnpressedBoxes.size()]);
		}
	}
	// Second verse, same as the first, this time with the sliders
	// Done twice for the bar and button it contains
	for (size_t i = 0; i < m_pcSliders.size(); i++)
	{
		rectangles.push_back(m_pcSliders[i]->GetBox());
		sprites.push_back(m_PressedBoxes[i % m_PressedBoxes.size()]);
		rectangles.push_back(m_pcSliders[i]->m_pcButtonToMove->GetButtonBox());
		sprites.push_back(m_UnpressedBoxes[i % m_UnpressedBoxes.size()]);
	}
	// Set the sprites to the spritebatch
	m_pd2dSpriteBatch->SetSprites(0, (UINT32)rectangles.size(), rectangles.data(), sprites.data());
}

ID2D1Bitmap1* CMenu::GetButtonSprite()
{
	return m_pd2dButtonSprite;
}

ID2D1SpriteBatch* CMenu::GetSpriteBatch()
{
	return m_pd2dSpriteBatch;
}
