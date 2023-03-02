#pragma once
#include "InputManager.h"
#include <windowsx.h>
#include <tchar.h>

#include "DebugDefines.h"

class CComponentManager;
class CEntityManager;
class CEventManager;
class CAudioManager;
class CRenderManager;
class CPhysicsManager;
class CLevelManager;
class CAIManager;
class CTime;

class CGameLogic : public ISystem
{
private:
	CComponentManager*			m_pcComponentManager;
	//CEntityManager*				m_pcEntityManager;
	static CEventManager*		m_pcEventManager;
	CPhysicsManager*			m_pcPhysicsManager;
	CAIManager*					m_pcAIManager;
	CTime*						m_pcTime;

	//static ParticleSystem*		m_pcParticleSystem;
	static CEntityManager*		m_pcEntityManager;
	static CAudioManager*		m_pcAudioManager;
	static CRenderManager*		m_pcRenderManager;
	static CLevelManager*		m_pcLevelManager;

	static int					m_nEnemiesKILLED;
	static int					m_nCurrentState;
	static bool					m_bStateInitialized;
	static bool					m_bMouseControls;

	static int floor;

	static float				m_fTimeoutTimer;

#ifdef CHEATCODES
	bool bGodMode = false;
	bool bAiGrid = false;
	bool bInfiniteMana = false;
	bool bRefillHealth = false;
#endif // CHEATCODES

	void LoadSettings();
	void SaveSettings();
	void OutLine();
	void CameraToPlayer();

	void BuildOptionsMenus();

	static void StartGame();
	static void ExitGame();
	static void TogglePause();
	static void GoBackToMain();
	static void PlayButtonSound();
	static void StartOptions();
	static void PauseOptions();
	static void StartControls();
	static void PauseControls();
	static void ToPause();
	static void ToStart();
	static void ToCredits();
	static void EndLevel();

	static void SwapControls();

	//	Use eEnemyType if you dont know the number
	void SpawnEnemy(int nEnemyType);

	//	Use eItemClass for the first Parameter if you don't know the number
	//	Use the corresponding enum for the class for the second item type
	void SpawnItem(int nItemClass, int nItemType);

public:
	CGameLogic(HWND cWindow);
	~CGameLogic();

	void Update();
	static void CheckLose();
	static void CheckWin();

	static int m_nSlain;
	static int m_nItemsCollected;
};
