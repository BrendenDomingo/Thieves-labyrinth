#pragma once
#include "GAudio.h"
#include "System.h"
#include "EnumTypes.h"


class CAudioManager : public ISystem
{
	#define SOUND_INSTANCES 10

	static GW::AUDIO::GAudio*	 m_pAudio;
	static GW::AUDIO::GSound*	 m_cSoundList[SOUND_INSTANCES][eSFX::COUNT];
	static GW::AUDIO::GMusic*	 m_cMusicList[eMusic::COUNT];

	static float m_nMusicVol, m_nSFXVol, m_nMasterVol;
	static int nCurrentMusic;

	bool m_pbPlayingSFX[SOUND_INSTANCES][eSFX::COUNT] = { false };
	bool m_bPaused;

public:
	CAudioManager();
	~CAudioManager();

	//	Initializes all music and sound effects, should be called after eventmanager is created.
	void Initialize();

	static void ShutDown();

	///////////////////////////////////////////////////////////////////
	// AddMusic()	 
	// in:	const char*, bool
	// out: void
	// adds music from passed in file path and adds it to a vector, plays with bool
	///////////////////////////////////////////////////////////////////
	void AddMusic(const char* _pchFilepath, int nMusicName, bool bPlay = false);

	///////////////////////////////////////////////////////////////////
	// AddSFX()	 
	// in:	const char*
	// out: void
	// adds SFX from passed in file path and adds it to a vector
	///////////////////////////////////////////////////////////////////
	void AddSFX(const char*,int);

	///////////////////////////////////////////////////////////////////
	// ChangeMusicVol()	 
	// in:	float
	// out: void
	// Changes music's vol from passed in float Value
	///////////////////////////////////////////////////////////////////
	static void ChangeMusicVol(float);

	///////////////////////////////////////////////////////////////////
	// ChangeSFXVol()	 
	// in:	float
	// out: void
	// Changes SFX's vol from passed in float Value
	///////////////////////////////////////////////////////////////////
	static void ChangeSFXVol(float);

	///////////////////////////////////////////////////////////////////
	// ChangeMasterVol()	 
	// in:	float
	// out: void
	// Changes Master vol from passed in float Value
	///////////////////////////////////////////////////////////////////
	static void ChangeMasterVol(float);

	///////////////////////////////////////////////////////////////////
	// ReceiveSoundRequest()	 
	// in:	bool, int, int (optional)
	// out: void
	// Prases sound request and plays music/sound
	///////////////////////////////////////////////////////////////////
	void ReceiveSoundRequest(bool bTrue, int nType, int nSFX = 0);

	///////////////////////////////////////////////////////////////////
	// ChangeCurrentMusic()	 
	// in:	int
	// out: void
	// Changes the current music to the music passed in using eMusic
	///////////////////////////////////////////////////////////////////
	static void ChangeCurrentMusic(int nMusic);

	///////////////////////////////////////////////////////////////////
	// ReturnCurrentMusic()	 
	// in:	void
	// out: int
	// returns the current music to using eMusic
	///////////////////////////////////////////////////////////////////
	int GetCurrentMusic();

	///////////////////////////////////////////////////////////////////
	// TogglePause()	 
	// in:	void
	// out: void
	// Pauses/unpauses sfx
	///////////////////////////////////////////////////////////////////
	void TogglePause();


	static float GetMasterVol();
	static float GetMusicVol();
	static float GetSFXVol();
};

