#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"


#include <queue>
#include <vector>

class CPlayerEntity;
class CRenderManager;
class CAudioManager;
class CEntityManager;
class CDebugManager;
class CGUIManager;

struct TDebugMessage
{
	TDebugMessage(std::string szMessage);
	std::string m_szMessageString;
	std::string GetDebugMessage();
};

struct TDebugLineMessage
{
	CMath::TVECTOR3 m_tPosition;
	CMath::TVECTOR4 m_tColor;
	TDebugLineMessage();
	TDebugLineMessage(CMath::TVECTOR3 _tPosition, CMath::TVECTOR4 _tColor);
};

struct TAudioMessage
{
	TAudioMessage(bool _bTrue, int _nType, int _nSound = 0);
	bool bTrue;
	int nType, nSound;
};

struct TEntityMessage
{
	TEntityMessage(int _nEntityID);
	int nEntityID;
};

struct TWindowResizeMessage
{
	TWindowResizeMessage(unsigned int _nWidth, unsigned int _nHeight);
	int nWidth;
	int nHeight;
};

struct TCollisionMessage
{
	TCollisionMessage(int _nCollider, int _nCollidingWith, int _nCollisionType);
	int nCollider;
	int nCollidingWidth;
	int nCollisionType;
};

struct THudMessage
{
	THudMessage(float _fCurrHealth, float _fMaxHealth, float _fCurrMana, float _fMaxMana);
	float fCurrHealth;
	float fMaxHealth;
	float fCurrMana;
	float fMaxMana;
};

class CEventManager : ISystem
{
	void ParseDebugMessage();
	void ParseDebugLineMessage();
	void ParseAudioMessage();
	void ParseCollissionMessage();
	void ParseResizeMessage();
	void ParseHUDMessage();
	
	static std::queue<TDebugMessage>*			m_cDebugMessages;
	static std::queue<TDebugLineMessage>*		m_cDebugLineMessages;
	static std::queue<TAudioMessage>*			m_cAudioMessages;
	static std::queue<TWindowResizeMessage>*	m_cResizeMessages;
	static std::queue<TCollisionMessage>*		m_cCollisionMessages;
	static std::queue<THudMessage>*				m_cHudMessages;

	CRenderManager*								m_pcRenderManager;
	CAudioManager*								m_pcAudioManager;
	CEntityManager*								m_pcEntityManager;

#ifdef MULTI_THREADING

	std::mutex									m_cMainMutex;
	std::mutex									m_cMutexLock;

	std::condition_variable						m_cGunCondition;
	std::condition_variable						m_cMainWait;

	std::vector<std::thread>					m_Threads;
	bool										m_bShutDownFlag;

	int											m_nThreadCount;
	int											m_nWorkingThreads;

#endif // MULTI_THREADING
public:
	void ClearMessages();


	CEventManager(CRenderManager* pcRenderer, CAudioManager* pcAudio, CEntityManager* pcEntity);
	~CEventManager();


	static void SendDebugMessage(TDebugMessage cMessage);

	static void SendDebugLineMessage(TDebugLineMessage cMessage);

	static void SendAudioMessage(TAudioMessage cMessage);

	static void SendResizeMessage(TWindowResizeMessage cMessage);

	static void SendCollisionMessage(TCollisionMessage cMessage);

	static IEntity* SendEntityMessage(TEntityMessage cMessage);

	static void SendHudMessage(THudMessage cMessage);

	void Notify();

	
};

