#pragma once
#include "HUD.h"
#include <dxgi1_2.h>
#include <string>
#include "WICTextureLoader.h"
#include "EntityManager.h"
#include "Component.h"
#include "FireDemonController.h"
#include "Stats.h"
#include "EnumTypes.h"
#include "Time.h"

int CHUD::m_nCurrConsume;
bool CHUD::m_bChange;
IEntity* CHUD::m_pcHoveredEntity;

void CHUD::UseHealth()
{
	m_nCurrConsume = eConsumableItems::HEALTH;
	m_bChange = true;
}

void CHUD::UseMana()
{
	m_nCurrConsume = eConsumableItems::MANA;
	m_bChange = true;
}

void CHUD::UseDamage()
{
	m_nCurrConsume = eConsumableItems::DAMAGE;
	m_bChange = true;
}

void CHUD::UseHaste()
{
	m_nCurrConsume = eConsumableItems::HASTE;
	m_bChange = true;
}

void CHUD::UseResist()
{
	m_nCurrConsume = eConsumableItems::RESIST;
	m_bChange = true;
}

void CHUD::Nothing()
{
	//Literally does nothing
	//I only added this to make a couple of things easier
}

CHUD::CHUD(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight)
{
	m_fCurrHealth = m_fCurrMana = 0.0f;
	// To avoid errors through dividing by 0
	m_fMaxHealth = m_fMaxMana = 1.0f;
	m_nCurrActive = m_nCurrConsume = 0;
	m_tCurrPass = { 0,0 };
	m_pnConsume = nullptr;
	m_pd2dTempSprite = m_pd2dInventorySprite = nullptr;
	m_pd2dInventoryBatch = nullptr;
	m_pcMenu = nullptr;
	m_pcHoveredEntity = nullptr;
	m_bChange = m_bRenderMenu = m_bDisplaySwap = false;
	m_fCurrBuffTime = { 0,0,0 };
	m_fMaxBuffTime = { 1,1,1 };
	pd2dContext->CreateSpriteBatch(&m_pd2dSpriteBatch);
	ID3D11Texture2D* fromDisk = nullptr;
	ID3D11ShaderResourceView* ignored = nullptr;
	//Sprite sheet will use this in the future
	if (FAILED(DirectX::CreateWICTextureFromFile(pd3dDevice, L"../Assets/Textures/UI/Sprite.png", (ID3D11Resource**)&fromDisk, &ignored)))
	{
		MessageBox(nullptr, L"Failed to load texture for buttons", L"Error", MB_OK);
		if (fromDisk)	fromDisk->Release();
		if (ignored)	ignored->Release();
		exit(0);
	}
	IDXGISurface2* texSurface = nullptr;
	fromDisk->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface);

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
	pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dTempSprite);
	texSurface->Release();
	if (fromDisk) { fromDisk->Release(); fromDisk = nullptr; }
	if (ignored) { ignored->Release();	 ignored = nullptr; }

	D2D1_RECT_F offscreen[22] = { 0, };
	m_pd2dSpriteBatch->AddSprites(22, offscreen);
	m_fCurrTime = CMath::TVECTOR3(0, 0, 0);
	m_fMaxTime = CMath::TVECTOR3(1, 1, 1);
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	m_RectPos.push_back(D2D1::RectU(0, 200, 199, 399)); // Ability 1
	m_RectPos.push_back(D2D1::RectU(200, 200, 399, 399)); // Ability 2
	m_RectPos.push_back(D2D1::RectU(400, 200, 599, 399)); // Ability 3
	m_RectPos.push_back(D2D1::RectU(0, 0, 199, 199)); // Health
	m_RectPos.push_back(D2D1::RectU(200, 0, 399, 199)); // Mana
	m_RectPos.push_back(D2D1::RectU(400, 0, 599, 199)); // Damage
	m_RectPos.push_back(D2D1::RectU(600, 0, 799, 199)); // Haste
	m_RectPos.push_back(D2D1::RectU(800, 0, 999, 199)); // Resistance
	m_RectPos.push_back(D2D1::RectU(600, 200, 799, 399)); // Passive Box
	m_RectPos.push_back(D2D1::RectU(800, 200, 999, 399)); // Active Box
	m_RectPos.push_back(D2D1::RectU(1000, 200, 1199, 399)); // Consumable Box
	m_RectPos.push_back(D2D1::RectU(0, 400, 199, 599)); // Ring - 11
	m_RectPos.push_back(D2D1::RectU(200, 400, 399, 599)); // Thimble
	m_RectPos.push_back(D2D1::RectU(400, 400, 599, 599)); // Belt
	m_RectPos.push_back(D2D1::RectU(600, 400, 799, 599)); // Gauntlet
	m_RectPos.push_back(D2D1::RectU(800, 400, 999, 599)); // Boots
	m_RectPos.push_back(D2D1::RectU(1000, 400, 1199, 599)); // Rune
	m_RectPos.push_back(D2D1::RectU(1200, 400, 1399, 599)); // Hat
	m_RectPos.push_back(D2D1::RectU(1400, 0, 1599, 199)); // Enemies Remaining
	m_RectPos.push_back(D2D1::RectU(1400, 200, 1599, 399)); // Total Enemies

	m_RectPos.push_back(D2D1::RectU(1000, 0, 1199, 199)); // Health and Mana Back
	m_RectPos.push_back(D2D1::RectU(1200, 0, 1399, 199)); // Health and Mana Box

	m_ButtonPos.push_back(D2D1::RectU(1200, 200, 1399, 399));
	m_ButtonPos.push_back(D2D1::RectU(800, 200, 999, 399));
	m_ButtonPos.push_back(D2D1::RectU(600, 200, 799, 399));
	m_ButtonPos.push_back(D2D1::RectU(1000, 200, 1199, 399));

	// Item Descriptions
	m_ItemDescriptions.push_back(L"This text should not be seen.  If this appears, please file this as a bug");
	// Consumables first
	m_ItemDescriptions.push_back(L"Health Potion:\nRestores 25% of your health");
	m_ItemDescriptions.push_back(L"Mana Potion:\nRestores all of your mana");
	m_ItemDescriptions.push_back(L"Damage Potion:\nIncreases damage dealt by all attacks by 50% for 20 seconds");
	m_ItemDescriptions.push_back(L"Speed Potion:\nIncreases movement speed by 25% for 20 seconds");
	m_ItemDescriptions.push_back(L"Resistance Potion:\nIncreases your defense by 25% for 20 seconds");
	// Passive Items next
	m_ItemDescriptions.push_back(L"Ring of Destruction:\nIncreases damage dealt by 15%");
	m_ItemDescriptions.push_back(L"Glorious Thimble of Defense:\nIncreases your defense by 50%");
	m_ItemDescriptions.push_back(L"Belt of Souls:\nRestores 5% of your max HP when you kill an enemy");
	m_ItemDescriptions.push_back(L"Power Gauntlet:\nIncreases damage dealt by 40% and decreases your defense by 20%");
	m_ItemDescriptions.push_back(L"Boots of Hermes:\nIncreases movement speed by 25%");
	m_ItemDescriptions.push_back(L"Rune of Haste:\nIncreases your attack speed by 20%");
	m_ItemDescriptions.push_back(L"Magical Hat:\nDecreases ability cooldown times by 25%");
	m_ItemDescriptions.push_back(L"This item is going to be dropped from the game.  If you see this, please file this as a bug");
}

void CHUD::Update(bool bInventory)
{
	if (bInventory)
	{
		for (size_t i = 0; i < m_pcMenu->m_pcButtons.size(); i++)
		{
			m_pcMenu->m_pcButtons[i]->Update();
		}
	}
	SetPlayerValues();
}

void CHUD::SetPlayerValues(float fCurrHealth, float fMaxHealth, float fCurrMana, float fMaxMana)
{
	IEntity* player = nullptr;
	player = CEntityManager::GetEntity(0);
	bool defaulted = false;
	if (player)
	{
		CStats* stats = player->GetComponent<CStats>();
		m_fCurrHealth = stats->GetHP();
		m_fMaxHealth = stats->GetMaxHP();
		m_fCurrMana = stats->GetMP();
		m_fMaxMana = stats->GetMaxMP();
		CInventory* inventory = player->GetComponent<CInventory>();
		m_nCurrActive = inventory->GetActive();
		m_tCurrPass = inventory->GetPassive();
		CFireDemonController* controller = player->GetComponent<CFireDemonController>();
		m_fMaxTime = CMath::TVECTOR3(controller->GetFireballCoolDown(), controller->GetAOECooldown(), controller->GetFirewallCooldown());
		m_fCurrTime = controller->GetAttackTime();
		m_fCurrBuffTime = controller->GetBuffTimes();
		m_fMaxBuffTime = controller->GetMaxBuffTime();
		m_bDisplaySwap = controller->IsOverItem();
		if (m_bChange)
		{
			inventory->SetCurrentConsumable(m_nCurrConsume);
			m_bChange = false;
		}
		// Value may not be what we set it to
		m_nCurrConsume = inventory->GetCurrentConsumable();

		// I'm aware that we don't need to reset this pointer
		// I'm doing this on the off-chance we implement another
		// controller or a new game is started after an old one ends
		inventory->GetConsumables(&m_pnConsume);
	}

	m_nEnemyCount = CLevel::GetTotalEnemyCount();
}

void CHUD::WriteHealthAndMana(ID2D1RenderTarget* pd2dRT, IDWriteFactory* pdwWriteFactory, IDWriteTextFormat* pdxwTextFormat, IDWriteTextFormat* pdxwTitleFormat, 
	ID2D1SolidColorBrush* pd2dBrushie, float fWidth, float fHeight, bool bControls)
{
	float scaleX = fWidth / 1024, scaleY = fHeight / 576;
	pd2dRT->BeginDraw();
	std::vector<std::wstring> toWrite;
	std::vector<D2D1_RECT_F> boxes;
	std::vector<D2D1_RECT_F> rectangles;
	std::wstring heal, mana, foes, room;
	rectangles.push_back(D2D1::RectF(fWidth * 0.01f, fHeight * 0.83f, fWidth * 0.09f, fHeight * 0.99f));
	rectangles.push_back(D2D1::RectF(fWidth * 0.11f, fHeight * 0.83f, fWidth * 0.19f, fHeight * 0.99f));
	rectangles.push_back(D2D1::RectF(fWidth * 0.21f, fHeight * 0.83f, fWidth * 0.29f, fHeight * 0.99f));
	//heal += L"Health: ";
	heal += std::to_wstring((int)m_fCurrHealth);
	heal += L" / ";
	heal += std::to_wstring((int)m_fMaxHealth);
	//mana += L"Mana: ";
	mana += std::to_wstring((int)m_fCurrMana);
	mana += L" / ";
	mana += std::to_wstring((int)m_fMaxMana);
	//foes += L"Total Enemies: ";
	foes += std::to_wstring(m_nEnemyCount);
	//room += L"Room Enemies: ";
	//room += std::to_wstring(CLevel::m_pcCurrentRoom->m_nEnemyCount);
	//toWrite.push_back(heal);
	//toWrite.push_back(mana);
	toWrite.push_back(foes);
	toWrite.push_back(room);
	//boxes.push_back(D2D1::RectF(0, 0, fWidth * 0.15f, fHeight * 0.05f));
	//boxes.push_back(D2D1::RectF(0, fHeight * 0.05f, fWidth * 0.15f, fHeight * 0.1f));
	boxes.push_back(D2D1::RectF(fWidth * 0.05f, fHeight * 0.1f, fWidth * 0.1f, fHeight * 0.2f));
	boxes.push_back(D2D1::RectF(fWidth * 0.05f, fHeight * 0.22f, fWidth * 0.1f, fHeight * 0.32f));
	IDWriteTextLayout* pdwTextLayout = nullptr;
	for (size_t i = 0; i < 2; i++)
	{
		D2D1_RECT_F box = boxes[i];
		float width = box.right - box.left, height = box.bottom - box.top;
		pdwWriteFactory->CreateTextLayout(
			toWrite[i].c_str(),
			(UINT32)toWrite[i].size(),
			pdxwTextFormat,
			width,
			height,
			&pdwTextLayout
		);
		pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left, box.top)));
		pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie);
		pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
		pdwTextLayout->Release();
		pdwTextLayout = nullptr;
	}

	toWrite.clear();
	if (bControls)
	{
		toWrite.push_back(L"Q");
		toWrite.push_back(L"W");
		toWrite.push_back(L"E");
	}
	else
	{
		toWrite.push_back(L"1");
		toWrite.push_back(L"2");
		toWrite.push_back(L"3");
	}

	if (m_nCurrConsume > eConsumableItems::NONE && m_pnConsume)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.91f, fHeight * 0.83f, fWidth * 0.99f, fHeight * 0.99f));
		toWrite.push_back(L"F");
	}

	if (m_bDisplaySwap)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.81f, fHeight * 0.83f, fWidth * 0.89f, fHeight * 0.99f));
		toWrite.push_back(L"Z");
		rectangles.push_back(D2D1::RectF(fWidth * 0.71f, fHeight * 0.83f, fWidth * 0.79f, fHeight * 0.99f));
		toWrite.push_back(L"X");
	}

	for (size_t i = 0; i < rectangles.size(); i++)
	{
		D2D1_RECT_F box = rectangles[i];
		float width = box.right - box.left, height = box.bottom - box.top;
		pdwWriteFactory->CreateTextLayout(
			toWrite[i].c_str(),
			(UINT32)toWrite[i].size(),
			pdxwTextFormat,
			width,
			height,
			&pdwTextLayout
		);
		pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left, box.top)));
		pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie);
		pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
		pdwTextLayout->Release();
		pdwTextLayout = nullptr;
	}

	if (m_dTime > 0)
	{
		m_dTime -= CTime::GetDelta();
		if (m_dTime <= 0)
		{
			m_dTime = 0;
		}
		else
		{
			std::wstring newGuy;
			newGuy += L"Eliminate all enemies in the dungeon!!";
			/*pd2dRT->DrawTextW(newGuy.c_str(), (UINT32)newGuy.size(), pdxwTitleFormat,
				D2D1::RectF(fWidth * 0.3f, fHeight * 0.15f, fWidth * 0.7f, fHeight * 0.35f), pd2dBrushie); */
			D2D1_RECT_F box = D2D1::RectF(fWidth * 0.3f, fHeight * 0.15f, fWidth * 0.7f, fHeight * 0.35f);
			float width = box.right - box.left, height = box.bottom - box.top;
			pdwWriteFactory->CreateTextLayout(
				newGuy.c_str(),
				(UINT32)newGuy.size(),
				pdxwTitleFormat,
				width,
				height,
				&pdwTextLayout
			);
			pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left + (width * 0.5f), box.top + (height * 0.5f))));
			pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie);
			pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
			pdwTextLayout->Release();
			pdwTextLayout = nullptr;
		}
	}
	pd2dRT->EndDraw();
}

void CHUD::SetMiniMap(std::vector<std::vector<Spot>> cMap)
{
	m_cMap = cMap;
	ResizeMiniMap(m_fWidth, m_fHeight);
}

void CHUD::ResizeMiniMap(float fWidth, float fHeight)
{
	m_rMap.clear();
	m_rMap.resize(m_cMap.size());
	float Left = fWidth * 0.5f;
	float Right = fWidth * 0.85f;
	float Top = fHeight * 0.1f;
	float Bottom = fHeight * 0.65f;
	for (size_t i = 0; i < m_rMap.size(); i++)
	{
		m_rMap[i].resize(m_cMap[i].size());
	}
	size_t col = m_rMap.size();
	for (size_t j = 0; j < col; j++)
	{
		size_t row = m_rMap[j].size();
		for (size_t i = 0; i < row; i++)
		{
			if (m_cMap[i][j].bRoom)
			{
				m_rMap[i][j] = D2D1::RectF(Left + ((Right - Left) * (i /(float)row)), Top + ((Bottom - Top) * ((col - j) / (float)col)),
					Left + ((Right - Left) * ((i + 1) / (float)row)), Top + ((Bottom - Top) * ((row - (j + 1)) / (float)col)));
			}
			else
			{
				m_rMap[i][j] = D2D1::RectF(0, 0, 0, 0);
			}
		}
	}
}

void CHUD::NewGame()
{
	m_dTime = 5;
}

CHUD::~CHUD()
{
	if (m_pd2dSpriteBatch)	{ m_pd2dSpriteBatch->Release();	m_pd2dSpriteBatch = nullptr; }
	if (m_pd2dTempSprite)	{ m_pd2dTempSprite->Release();	m_pd2dTempSprite = nullptr; }
	if (m_pd2dInventoryBatch) { m_pd2dInventoryBatch->Release();	m_pd2dInventoryBatch = nullptr; }
	if (m_pd2dInventorySprite) { m_pd2dInventorySprite->Release();	m_pd2dInventorySprite = nullptr; }
	if (m_pcMenu)	delete m_pcMenu;
}

void CHUD::DrawBaseHUD(ID2D1DeviceContext3* pd2dContext, ID2D1RenderTarget* pd2dRT, ID2D1SolidColorBrush* pd2dBrushie, ID2D1SolidColorBrush* pd2dBrushie2, ID2D1SolidColorBrush* pd2dBrushie3, float fWidth, float fHeight)
{
	// Set up base for the HUD, and draw it immediately
	std::vector<D2D1_RECT_F> rectangles;
	std::vector<D2D1_RECT_F> hold;
	std::vector<D2D1_RECT_U> test;
	std::vector<D2D1_COLOR_F> colors;

	#pragma region Abilities

	rectangles.push_back(D2D1::RectF(fWidth * 0.01f, fHeight * 0.83f, fWidth * 0.09f, fHeight * 0.99f));
	rectangles.push_back(D2D1::RectF(fWidth * 0.11f, fHeight * 0.83f, fWidth * 0.19f, fHeight * 0.99f));
	rectangles.push_back(D2D1::RectF(fWidth * 0.21f, fHeight * 0.83f, fWidth * 0.29f, fHeight * 0.99f));
	test.push_back(m_RectPos[0]);
	test.push_back(m_RectPos[1]);
	test.push_back(m_RectPos[2]);
	UINT32 count = (UINT32)rectangles.size();
	m_pd2dSpriteBatch->SetSprites(0, count, rectangles.data(), test.data());

	// New Ability Cooldown Stuff
	// Get the ratios of the cooldowns
	float ratios[3];
	ratios[0] = m_fCurrTime.x / m_fMaxTime.x;
	ratios[1] = m_fCurrTime.y / m_fMaxTime.y;
	ratios[2] = m_fCurrTime.z / m_fMaxTime.z;

	for (size_t i = 0; i < 3; i++)
	{
		// Get Sprite Positions
		D2D1_RECT_F rectangle = rectangles[i];
		D2D1_RECT_U other = test[i];
		// Update them
		rectangle.top = rectangle.bottom - ((rectangle.bottom - rectangle.top) * ratios[i]);
		other.top = other.bottom - (UINT32)((other.bottom - other.top) * ratios[i]);
		// Change Vectors to new values
		rectangles[i] = rectangle;
		test[i] = other;
		// Add a Gray for coloring cooldowns
		D2D1_COLOR_F color;
		color.r = color.g = color.b = 0.2f;
		color.a = 1.0f;
		colors.push_back(color);
		hold.push_back(rectangles[i]);
	}

	m_pd2dSpriteBatch->SetSprites(count, (UINT32)rectangles.size(), rectangles.data(), test.data(), colors.data());
	count += (UINT32)rectangles.size();

	rectangles.clear();
	test.clear();
	colors.clear();

	#pragma endregion

	#pragma region Items

	// That was an attempt to do things with different alpha values in the future
	if (m_nCurrConsume > eConsumableItems::NONE)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.91f, fHeight * 0.83f, fWidth * 0.99f, fHeight * 0.99f));
		rectangles.push_back(rectangles[rectangles.size() - 1]);
		colors.push_back({ 1,1,1,1 });
		colors.push_back({ 1,1,1,1 });
		switch (m_nCurrConsume)
		{
		case eConsumableItems::HEALTH:
		{
			test.push_back(m_RectPos[10]);
			test.push_back(m_RectPos[3]);
			rectangles.push_back({ 0 });
			rectangles.push_back({ 0 });
			test.push_back({ 0 });
			test.push_back({ 0 });
			colors.push_back({ 0 });
			colors.push_back({ 0 });
			break;
		}
		case eConsumableItems::MANA:
		{
			test.push_back(m_RectPos[10]);
			test.push_back(m_RectPos[4]);
			rectangles.push_back({ 0 });
			rectangles.push_back({ 0 });
			test.push_back({ 0 });
			test.push_back({ 0 });
			colors.push_back({ 0 });
			colors.push_back({ 0 });
			break;
		}
		case eConsumableItems::DAMAGE:
		{
			test.push_back(m_RectPos[10]);
			test.push_back(m_RectPos[5]);
			float ratio = fmaxf(0.0f, fminf(1.0f, m_fCurrBuffTime.x / m_fMaxBuffTime.x));
			D2D1_RECT_F boxF = rectangles[rectangles.size() - 1];
			D2D1_RECT_U boxU1 = m_RectPos[10];
			D2D1_RECT_U boxU2 = m_RectPos[5];
			boxF.top = boxF.bottom - ((boxF.bottom - boxF.top) * ratio);
			boxU1.top = boxU1.bottom - (UINT32)((boxU1.bottom - boxU1.top) * ratio);
			boxU2.top = boxU2.bottom - (UINT32)((boxU2.bottom - boxU2.top) * ratio);
			rectangles.push_back(boxF);
			rectangles.push_back(boxF);
			test.push_back(boxU1);
			test.push_back(boxU2);
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::DarkOrange);
			color.a = 0.3f;
			colors.push_back(color);	colors.push_back(color);
			break;
		}
		case eConsumableItems::HASTE:
		{
			test.push_back(m_RectPos[10]);
			test.push_back(m_RectPos[6]);
			float ratio = fmaxf(0.0f, fminf(1.0f, m_fCurrBuffTime.y / m_fMaxBuffTime.y));
			D2D1_RECT_F boxF = rectangles[rectangles.size() - 1];
			D2D1_RECT_U boxU1 = m_RectPos[10];
			D2D1_RECT_U boxU2 = m_RectPos[6];
			boxF.top = boxF.bottom - ((boxF.bottom - boxF.top) * ratio);
			boxU1.top = boxU1.bottom - (UINT32)((boxU1.bottom - boxU1.top) * ratio);
			boxU2.top = boxU2.bottom - (UINT32)((boxU2.bottom - boxU2.top) * ratio);
			rectangles.push_back(boxF);
			rectangles.push_back(boxF);
			test.push_back(boxU1);
			test.push_back(boxU2);
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Purple);
			color.a = 0.3f;
			colors.push_back(color);	colors.push_back(color);
			break;
		}
		case eConsumableItems::RESIST:
		{
			test.push_back(m_RectPos[10]);
			test.push_back(m_RectPos[7]);
			float ratio = fmaxf(0.0f, fminf(1.0f, m_fCurrBuffTime.z / m_fMaxBuffTime.z));
			D2D1_RECT_F boxF = rectangles[rectangles.size() - 1];
			D2D1_RECT_U boxU1 = m_RectPos[10];
			D2D1_RECT_U boxU2 = m_RectPos[7];
			boxF.top = boxF.bottom - ((boxF.bottom - boxF.top) * ratio);
			boxU2.top = boxU2.bottom - (UINT32)((boxU2.bottom - boxU2.top) * ratio);
			boxU1.top = boxU1.bottom - (UINT32)((boxU1.bottom - boxU1.top) * ratio);
			rectangles.push_back(boxF);
			rectangles.push_back(boxF);
			test.push_back(boxU1);
			test.push_back(boxU2);
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Green);
			color.a = 0.3f;
			colors.push_back(color);	colors.push_back(color);
			break;
		}
		default:
		{
			test.push_back(D2D1::RectU(0, 0, 0, 0));
			test.push_back(D2D1::RectU(0, 0, 0, 0));
			rectangles.push_back({ 0 });
			rectangles.push_back({ 0 });
			test.push_back({ 0 });
			test.push_back({ 0 });
			colors.push_back({ 0 });
			colors.push_back({ 0 });
			break;
		}
		}

	}
	else
	{
		rectangles.push_back({ 0 });
		rectangles.push_back({ 0 });
		test.push_back({ 0 });
		test.push_back({ 0 });
		colors.push_back({ 0 });
		colors.push_back({ 0 });
		rectangles.push_back({ 0 });
		rectangles.push_back({ 0 });
		test.push_back({ 0 });
		test.push_back({ 0 });
		colors.push_back({ 0 });
		colors.push_back({ 0 });
	}
	if ((int)m_tCurrPass.y > ePassiveItems::NONE)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.71f, fHeight * 0.83f, fWidth * 0.79f, fHeight * 0.99f));
		test.push_back(m_RectPos[9]);
		colors.push_back({ 1,1,1,1 });
		rectangles.push_back(D2D1::RectF(fWidth * 0.71f, fHeight * 0.83f, fWidth * 0.79f, fHeight * 0.99f));
		switch ((int)m_tCurrPass.y)
		{
		case ePassiveItems::RING:
		{
			test.push_back(m_RectPos[11]);
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			test.push_back(m_RectPos[12]);
			break;
		}
		case ePassiveItems::BELT:
		{
			test.push_back(m_RectPos[13]);
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			test.push_back(m_RectPos[14]);
			break;
		}
		case ePassiveItems::BOOTS:
		{
			test.push_back(m_RectPos[15]);
			break;
		}
		case ePassiveItems::HASTE:
		{
			test.push_back(m_RectPos[16]);
			break;
		}
		case ePassiveItems::HAT:
		{
			test.push_back(m_RectPos[17]);
			break;
		}
		default:
			test.push_back(D2D1::RectU(0, 0, 0, 0));
			break;
		}
		colors.push_back({ 1,1,1,1 });
	}
	else
	{
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
		test.push_back(D2D1::RectU(0, 0, 0, 0));
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
		test.push_back(D2D1::RectU(0, 0, 0, 0));
		colors.push_back({ 0,0,0,0 });
		colors.push_back({ 0,0,0,0 });
	}
	if ((int)m_tCurrPass.x > ePassiveItems::NONE)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.81f, fHeight * 0.83f, fWidth * 0.89f, fHeight * 0.99f));
		test.push_back(m_RectPos[8]);
		rectangles.push_back(D2D1::RectF(fWidth * 0.81f, fHeight * 0.83f, fWidth * 0.89f, fHeight * 0.99f));
		switch ((int)m_tCurrPass.x)
		{
		case ePassiveItems::RING:
		{
			test.push_back(m_RectPos[11]);
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			test.push_back(m_RectPos[12]);
			break;
		}
		case ePassiveItems::BELT:
		{
			test.push_back(m_RectPos[13]);
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			test.push_back(m_RectPos[14]);
			break;
		}
		case ePassiveItems::BOOTS:
		{
			test.push_back(m_RectPos[15]);
			break;
		}
		case ePassiveItems::HASTE:
		{
			test.push_back(m_RectPos[16]);
			break;
		}
		case ePassiveItems::HAT:
		{
			test.push_back(m_RectPos[17]);
			break;
		}
		default:
			test.push_back(D2D1::RectU(0, 0, 0, 0));
			break;
		}
		colors.push_back({ 1,1,1,1 });
		colors.push_back({ 1,1,1,1 });
	}
	else
	{
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
		test.push_back(D2D1::RectU(0, 0, 0, 0));
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
		test.push_back(D2D1::RectU(0, 0, 0, 0));
		colors.push_back({ 0,0,0,0 });
		colors.push_back({ 0,0,0,0 });
	}


	m_pd2dSpriteBatch->SetSprites(count, (UINT32)rectangles.size(), rectangles.data(), test.data(), colors.data());
	count += (UINT32)rectangles.size();

	#pragma endregion

	#pragma region Health and Mana

	// Health and Mana rendering
	rectangles.clear();
	test.clear();
	colors.clear();

	D2D1_RECT_U healBox = m_RectPos[m_RectPos.size() - 1];
	D2D1_RECT_U manaBox = healBox;
	D2D1_RECT_F heal = D2D1::RectF(fWidth * 0.001f, fHeight * 0.001f, fWidth * 0.249f, fHeight * 0.049f);
	D2D1_RECT_F mana = D2D1::RectF(fWidth * 0.001f, fHeight * 0.052f, fWidth * 0.249f, fHeight * 0.099f);
	float ratioHealth = (m_fCurrHealth / m_fMaxHealth), ratioMana = (m_fCurrMana / m_fMaxMana);
	heal.right = heal.left + ((heal.right - heal.left) * fmaxf(0, fminf(1, ratioHealth)));
	mana.right = mana.left + ((mana.right - mana.left) * fmaxf(0, fminf(1, ratioMana)));
	healBox.right = healBox.left + (UINT32)((healBox.right - healBox.left) * fmaxf(0, fminf(1, ratioHealth)));
	manaBox.right = manaBox.left + (UINT32)((manaBox.right - manaBox.left) * fmaxf(0, fminf(1, ratioMana)));
	rectangles.push_back(heal);
	rectangles.push_back(mana);
	test.push_back(healBox);
	test.push_back(manaBox);
	D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Red);
	colors.push_back(color);
	color = D2D1::ColorF(D2D1::ColorF::Blue);
	colors.push_back(color);
	test.push_back(m_RectPos[m_RectPos.size() - 2]);
	test.push_back(m_RectPos[m_RectPos.size() - 2]);
	rectangles.push_back(D2D1::RectF(0, 0, fWidth * 0.25f, fHeight * 0.049f));
	rectangles.push_back(D2D1::RectF(0, fHeight * 0.051f, fWidth * 0.25f, fHeight * 0.1f));
	colors.push_back({ 1,1,1,1 });
	colors.push_back({ 1,1,1,1 });

	m_pd2dSpriteBatch->SetSprites(count, (UINT32)rectangles.size(), rectangles.data(), test.data(), colors.data());
	count += (UINT32)rectangles.size();

	#pragma endregion

	#pragma region Enemy Stuff

	rectangles.clear();
	test.clear();
	colors.clear();

	if (m_pcHoveredEntity)
	{
		if (m_pcHoveredEntity->m_nEntityType == eEntity::ENEMY)
		{
			CStats* stats = m_pcHoveredEntity->GetComponent<CStats>();
			float max = stats->GetMaxHP(), curr = stats->GetHP(), left = fWidth * 0.8f;
			float ratio = fmaxf(0.0f, fminf(1.0f, curr / max));
			D2D1_RECT_F back = D2D1::RectF(left, 0, fWidth, fHeight * 0.03f);
			left = fWidth * 0.801f;
			heal = D2D1::RectF(left, 0, left + ((fWidth * 0.999f - left) * ratio), back.bottom);
			D2D1_RECT_U ground = m_RectPos[m_RectPos.size() - 1];
			ground.right = ground.left + (UINT32)((ground.right - ground.left) * ratio);
			test.push_back(ground);
			test.push_back(m_RectPos[m_RectPos.size() - 2]);
			rectangles.push_back(heal);
			rectangles.push_back(back);
			colors.push_back({ 1,0,0,1 });
			colors.push_back({ 1,1,1,1 });
		}
		else
		{
			rectangles.push_back({ 0 });
			rectangles.push_back({ 0 });
			test.push_back({ 0 });
			test.push_back({ 0 });
			colors.push_back({ 0 });
			colors.push_back({ 0 });
		}
	}
	else
	{
		rectangles.push_back({ 0 });
		rectangles.push_back({ 0 });
		test.push_back({ 0 });
		test.push_back({ 0 });
		colors.push_back({ 0 });
		colors.push_back({ 0 });
	}

	m_pd2dSpriteBatch->SetSprites(count, (UINT32)rectangles.size(), rectangles.data(), test.data(), colors.data());
	count += (UINT32)rectangles.size();

	#pragma endregion

	#pragma region Enemy Display

	rectangles.clear();
	test.clear();
	rectangles.push_back(D2D1::RectF(0, fHeight * 0.1f, fWidth * 0.05f, fHeight * 0.2f));
	rectangles.push_back(D2D1::RectF(0, fHeight * 0.22f, fWidth * 0.05f, fHeight * 0.32f));
	test.push_back(m_RectPos[m_RectPos.size() - 3]);
	test.push_back(m_RectPos[m_RectPos.size() - 4]);
	m_pd2dSpriteBatch->SetSprites(count, (UINT32)rectangles.size(), rectangles.data(), test.data());
	count += (UINT32)rectangles.size();

	#pragma endregion

	pd2dContext->BeginDraw();
	pd2dContext->DrawSpriteBatch(m_pd2dSpriteBatch, m_pd2dTempSprite);
	pd2dContext->EndDraw();
}

void CHUD::SetMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	if (!m_pcMenu)
	{
		m_pcMenu = new CMenu(pd3dDevice, pd2dContext);
		m_pcMenu->AddButton(UseHealth, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.15f, 0.25f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseMana, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.3f, 0.4f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseDamage, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.45f, 0.55f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseHaste, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.6f, 0.7f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseResist, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.75f, 0.85f, 0.7f, 0.9f);
		m_pcMenu->AddButton(Nothing, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.15f, 0.25f, 0.4f, 0.6f);
		m_pcMenu->AddButton(Nothing, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.3f, 0.4f, 0.4f, 0.6f);

		pd2dContext->CreateSpriteBatch(&m_pd2dInventoryBatch);
		D2D1_RECT_F offscreen[13] = { 0, };
		m_pd2dInventoryBatch->AddSprites(13, offscreen);
	}
}

void CHUD::DrawInventory(ID2D1DeviceContext3* pd2dContext, ID2D1RenderTarget* pd2dRT, IDWriteFactory* pdwWriteFactory, IDWriteTextFormat* pdxwTextFormat, IDWriteTextFormat* pdxwTitleFormat,
	ID2D1SolidColorBrush* pd2dBrushie, ID2D1SolidColorBrush* pd2dBrushie2, ID2D1SolidColorBrush* pd2dBrushie3, ID2D1SolidColorBrush* pd2dBrushie4, 
	ID2D1SolidColorBrush* pd2dBrushie5, float fWidth, float fHeight)
{
	float scaleX = fWidth / 1024.0f, scaleY = fHeight / 576.0f;
	pd2dRT->BeginDraw();
	pd2dRT->FillRectangle(D2D1::RectF(fWidth * 0.1f, fHeight * 0.05f, fWidth * 0.9f, fHeight * 0.95f), pd2dBrushie);
	IDWriteTextLayout* pdwTextLayout = nullptr;
	D2D1_RECT_F box = D2D1::RectF(fWidth * 0.1f, fHeight * 0.05f, fWidth * 0.45f, fHeight * 0.35f);
	float width = box.right - box.left, height = box.bottom - box.top;
	pdwWriteFactory->CreateTextLayout(
		L"Inventory",
		9,
		pdxwTitleFormat,
		width,
		height,
		&pdwTextLayout
	);
	pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left + (width * 0.5f), box.top + (height * 0.5f))));
	pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie2); 
	pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
	pdwTextLayout->Release();
	pdwTextLayout = nullptr;
	//pd2dRT->DrawTextW(L"Inventory", 9, pdxwTitleFormat, D2D1::RectF(fWidth * 0.1f, fHeight * 0.05f, fWidth * 0.45f, fHeight * 0.35f), pd2dBrushie2);
	pd2dRT->EndDraw();
	pd2dContext->BeginDraw();
	m_pcMenu->SetSprites();
	pd2dContext->DrawSpriteBatch(m_pcMenu->GetSpriteBatch(), m_pcMenu->GetButtonSprite());
	std::vector<D2D1_RECT_F> rectangular;
	std::vector<D2D1_RECT_U> locale;
	std::vector<D2D1_COLOR_F> colors;
	if (m_pnConsume)
	{
		if (m_pnConsume[1] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[0]->GetButtonBox());
			locale.push_back(m_RectPos[3]);
			colors.push_back({ 1,1,1,1 });
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
		}
		if (m_pnConsume[2] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[1]->GetButtonBox());
			locale.push_back(m_RectPos[4]);
			colors.push_back({ 1,1,1,1 });
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
		}
		if (m_pnConsume[3] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[2]->GetButtonBox());
			locale.push_back(m_RectPos[5]);
			colors.push_back({ 1,1,1,1 });
			float ratio = fmaxf(0.0f, fminf(1.0f, m_fCurrBuffTime.x / m_fMaxBuffTime.x));
			D2D1_RECT_F boxF = rectangular[rectangular.size() - 1];
			D2D1_RECT_U boxU1 = m_ButtonPos[2 % m_ButtonPos.size()];
			D2D1_RECT_U boxU2 = m_RectPos[5];
			boxF.top = boxF.bottom - ((boxF.bottom - boxF.top) * ratio);
			boxU2.top = boxU2.bottom - (UINT32)((boxU2.bottom - boxU2.top) * ratio);
			boxU1.top = boxU1.bottom - (UINT32)((boxU1.bottom - boxU1.top) * ratio);
			rectangular.push_back(boxF);
			rectangular.push_back(boxF);
			locale.push_back(boxU1);
			locale.push_back(boxU2);
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::DarkOrange);
			color.a = 0.3f;
			colors.push_back(color);	colors.push_back(color);
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
		}
		if (m_pnConsume[4] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[3]->GetButtonBox());
			locale.push_back(m_RectPos[6]);
			colors.push_back({ 1,1,1,1 });
			float ratio = fmaxf(0.0f, fminf(1.0f, m_fCurrBuffTime.y / m_fMaxBuffTime.y));
			D2D1_RECT_F boxF = rectangular[rectangular.size() - 1];
			D2D1_RECT_U boxU1 = m_ButtonPos[3 % m_ButtonPos.size()];
			D2D1_RECT_U boxU2 = m_RectPos[6];
			boxF.top = boxF.bottom - ((boxF.bottom - boxF.top) * ratio);
			boxU2.top = boxU2.bottom - (UINT32)((boxU2.bottom - boxU2.top) * ratio);
			boxU1.top = boxU1.bottom - (UINT32)((boxU1.bottom - boxU1.top) * ratio);
			rectangular.push_back(boxF);
			rectangular.push_back(boxF);
			locale.push_back(boxU1);
			locale.push_back(boxU2);
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Purple);
			color.a = 0.3f;
			colors.push_back(color);	colors.push_back(color);
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
		}
		if (m_pnConsume[5] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[4]->GetButtonBox());
			locale.push_back(m_RectPos[7]);
			colors.push_back({ 1,1,1,1 });
			float ratio = fmaxf(0.0f, fminf(1.0f, m_fCurrBuffTime.z / m_fMaxBuffTime.z));
			D2D1_RECT_F boxF = rectangular[rectangular.size() - 1];
			D2D1_RECT_U boxU1 = m_ButtonPos[4 % m_ButtonPos.size()];
			D2D1_RECT_U boxU2 = m_RectPos[7];
			boxF.top = boxF.bottom - ((boxF.bottom - boxF.top) * ratio);
			boxU2.top = boxU2.bottom - (UINT32)((boxU2.bottom - boxU2.top) * ratio);
			boxU1.top = boxU1.bottom - (UINT32)((boxU1.bottom - boxU1.top) * ratio);
			rectangular.push_back(boxF);
			rectangular.push_back(boxF);
			locale.push_back(boxU1);
			locale.push_back(boxU2);
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Green);
			color.a = 0.3f;
			colors.push_back(color);	colors.push_back(color);
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
		}
	}
	else
	{
		for (size_t i = 0; i < 11; i++)
		{
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			colors.push_back({ 0 });
		}
	}
	if ((int)m_tCurrPass.y > ePassiveItems::NONE) {
		rectangular.push_back(m_pcMenu->m_pcButtons[5]->GetButtonBox());
		colors.push_back({ 1,1,1,1 });
		switch ((int)m_tCurrPass.y)
		{
		case ePassiveItems::RING:
		{
			locale.push_back(m_RectPos[11]);
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			locale.push_back(m_RectPos[12]);
			break;
		}
		case ePassiveItems::BELT:
		{
			locale.push_back(m_RectPos[13]);
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			locale.push_back(m_RectPos[14]);
			break;
		}
		case ePassiveItems::BOOTS:
		{
			locale.push_back(m_RectPos[15]);
			break;
		}
		case ePassiveItems::HASTE:
		{
			locale.push_back(m_RectPos[16]);
			break;
		}
		case ePassiveItems::HAT:
		{
			locale.push_back(m_RectPos[17]);
			break;
		}
		default:
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			break;
		}
	}
	else {
		rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		locale.push_back(D2D1::RectU(0, 0, 0, 0));
		colors.push_back({ 0 });
	}
	if ((int)m_tCurrPass.x > ePassiveItems::NONE) {
		rectangular.push_back(m_pcMenu->m_pcButtons[6]->GetButtonBox());
		colors.push_back({ 1,1,1,1 });
		switch ((int)m_tCurrPass.x)
		{
		case ePassiveItems::RING:
		{
			locale.push_back(m_RectPos[11]);
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			locale.push_back(m_RectPos[12]);
			break;
		}
		case ePassiveItems::BELT:
		{
			locale.push_back(m_RectPos[13]);
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			locale.push_back(m_RectPos[14]);
			break;
		}
		case ePassiveItems::BOOTS:
		{
			locale.push_back(m_RectPos[15]);
			break;
		}
		case ePassiveItems::HASTE:
		{
			locale.push_back(m_RectPos[16]);
			break;
		}
		case ePassiveItems::HAT:
		{
			locale.push_back(m_RectPos[17]);
			break;
		}
		default:
			locale.push_back(D2D1::RectU(0, 0, 0, 0));
			break;
		}
	}
	else {
		rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		locale.push_back(D2D1::RectU(0, 0, 0, 0));
		colors.push_back({ 0 });
	}

	// TODO: after the spritesheet is created
	// pull the locations for each item appropriately
	// Also set the colors for consumable items appropriately
	m_pd2dInventoryBatch->SetSprites(0, (UINT32)rectangular.size(), rectangular.data(), locale.data(), colors.data());
	pd2dContext->DrawSpriteBatch(m_pd2dInventoryBatch, m_pd2dTempSprite);
	pd2dContext->EndDraw();
	pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
	pd2dRT->BeginDraw();
	std::wstring number;
	if (m_pnConsume)
	{
		for (size_t i = 0; i < 5; i++)
		{
			number.clear();
			number += std::to_wstring(m_pnConsume[i + 1]);
			box = m_pcMenu->m_pcButtons[i]->GetButtonBox();
			width = box.right - box.left, height = box.bottom - box.top;
			pdwWriteFactory->CreateTextLayout(
				number.c_str(),
				(UINT32)number.size(),
				pdxwTextFormat,
				width,
				height,
				&pdwTextLayout
			);
			pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.right, box.bottom)));
			pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie3);
			pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
			pdwTextLayout->Release();
			pdwTextLayout = nullptr;
		}
	}
	else
	{
		number += std::to_wstring(0);
		for (size_t i = 0; i < 5; i++)
		{
			box = m_pcMenu->m_pcButtons[i]->GetButtonBox();
			width = box.right - box.left, height = box.bottom - box.top;
			pdwWriteFactory->CreateTextLayout(
				number.c_str(),
				1,
				pdxwTextFormat,
				width,
				height,
				&pdwTextLayout
			);
			pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.right, box.bottom)));
			pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie5);
			pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
			pdwTextLayout->Release();
			pdwTextLayout = nullptr;
		}
	}
	float Left = fWidth * 0.5f;
	float Right = fWidth * 0.85f;
	float Top = fHeight * 0.1f;
	float Bottom = fHeight * 0.65f;
	float CenterX = Left + ((Right - Left) * 0.5f);
	float CenterY = Top + ((Bottom - Top) * 0.5f);
	pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::SizeF(0.7f, 0.7f), D2D1::Point2F(CenterX, CenterY)) * D2D1::Matrix3x2F::Rotation(-45, D2D1::Point2F(CenterX, CenterY)));
	size_t col = m_rMap.size();
	ID2D1SolidColorBrush* orange = nullptr;
	pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &orange);
	for (size_t j = 0; j < col; j++)
	{
		size_t row = m_rMap[j].size();
		for (size_t i = 0; i < row; i++)
		{
			if (CLevel::m_pcCurrentRoom == m_cMap[j][i].Room)
			{
				pd2dRT->FillRectangle(m_rMap[j][i], pd2dBrushie2);
			}
			else if (m_cMap[j][i].Room)
			{
				if (m_cMap[j][i].Room->Visited)
				{
					pd2dRT->FillRectangle(m_rMap[j][i], orange);
				}
				else
				{
					std::vector<CRoomEntity*> neighbors = m_cMap[j][i].Room->m_pcNeighbors;
					for (size_t k = 0; k < neighbors.size(); k++)
					{
						if (neighbors[k]->Visited)
						{
							pd2dRT->FillRectangle(m_rMap[j][i], pd2dBrushie5);
						}
					}
				}
			}
		}
	}
	orange->Release();
	pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
	if (m_pnConsume)
	{
		for (int i = 1; i < eConsumableItems::COUNT; i++)
		{
			if (m_pcMenu->m_pcButtons[i - 1]->IsHovered() && m_pnConsume[i] > 0)
			{
				box = m_pcMenu->m_pcButtons[i - 1]->GetButtonBox();
				width = box.right - box.left;
				height = box.bottom - box.top;
				pd2dRT->FillRectangle(box, pd2dBrushie);
				pdwWriteFactory->CreateTextLayout(
					m_ItemDescriptions[i].c_str(),
					(UINT32)m_ItemDescriptions[i].size(),
					pdxwTitleFormat,
					width,
					height,
					&pdwTextLayout
				);
				//pd2dRT->DrawTextW(m_ItemDescriptions[i].c_str(), (UINT32)m_ItemDescriptions[i].size(), pdxwTitleFormat, box, pd2dBrushie2);
				pdwTextLayout->SetFontSize(15.0f * (width / height), { 0, (UINT32)m_ItemDescriptions[i].size() });
				pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left + (width * 0.5f), box.top + (height * 0.5f))));
				pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie2);
				pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
				pdwTextLayout->Release();
				pdwTextLayout = nullptr;
			}
		}

		if ((int)m_tCurrPass.x > ePassiveItems::NONE)
		{
			if (m_pcMenu->m_pcButtons[6]->IsHovered())
			{
				box = m_pcMenu->m_pcButtons[6]->GetButtonBox();
				width = box.right - box.left;
				height = box.bottom - box.top;
				pd2dRT->FillRectangle(box, pd2dBrushie);
				pdwWriteFactory->CreateTextLayout(
				m_ItemDescriptions[5 + (int)m_tCurrPass.x].c_str(),
				(UINT32)m_ItemDescriptions[5 + (int)m_tCurrPass.x].size(),
				pdxwTitleFormat,
				width,
				height,
				&pdwTextLayout
				);
				//pd2dRT->DrawTextW(m_ItemDescriptions[i].c_str(), (UINT32)m_ItemDescriptions[i].size(), pdxwTitleFormat, box, pd2dBrushie2);
				pdwTextLayout->SetFontSize(15.0f * (width / height), { 0, (UINT32)m_ItemDescriptions[5 + (int)m_tCurrPass.x].size() });
				pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left + (width * 0.5f), box.top + (height * 0.5f))));
				pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie2);
				pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
				pdwTextLayout->Release();
				pdwTextLayout = nullptr;
			}
		}

		if ((int)m_tCurrPass.y > ePassiveItems::NONE)
		{
			if (m_pcMenu->m_pcButtons[5]->IsHovered())
			{
				box = m_pcMenu->m_pcButtons[5]->GetButtonBox();
				width = box.right - box.left;
				height = box.bottom - box.top;
				pd2dRT->FillRectangle(box, pd2dBrushie);
				pdwWriteFactory->CreateTextLayout(
					m_ItemDescriptions[5 + (int)m_tCurrPass.y].c_str(),
					(UINT32)m_ItemDescriptions[5 + (int)m_tCurrPass.y].size(),
					pdxwTitleFormat,
					width,
					height,
					&pdwTextLayout
				);
				//pd2dRT->DrawTextW(m_ItemDescriptions[i].c_str(), (UINT32)m_ItemDescriptions[i].size(), pdxwTitleFormat, box, pd2dBrushie2);
				pdwTextLayout->SetFontSize(15.0f * (width / height), { 0, (UINT32)m_ItemDescriptions[5 + (int)m_tCurrPass.y].size() });
				pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY }, D2D1::Point2F(box.left + (width * 0.5f), box.top + (height * 0.5f))));
				pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, pd2dBrushie2);
				pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
				pdwTextLayout->Release();
				pdwTextLayout = nullptr;
			}
		}
	}

	pd2dRT->EndDraw();
}

void CHUD::ResizeItems(float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	m_pcMenu->ResizeItems(fWidth, fHeight, fwWidth, fwHeight);
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	ResizeMiniMap(fWidth, fHeight);
}
