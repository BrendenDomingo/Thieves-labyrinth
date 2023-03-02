#pragma once
#include "System.h"
#include <vector>
#include <fstream>
#include <DirectXMath.h>
//#include "EnumTypes.h"

#define MAX_JOINTS 35

using namespace DirectX;

class CAnimatorComponent;
struct ID3D11Device;
struct ID3D11ShaderResourceView;

class CAnimator
{
public:
	
	struct TAnimationClip
	{
		double m_dDuration;
		int	   m_nFrameCount;
	};

	int					m_nAnimationCount;
	TAnimationClip*		m_ptAnimationClips;

	int					m_nJointCount;
	XMMATRIX*			m_ptBindPoseMatrices;

	int					m_nMaxFrameCount;

	ID3D11ShaderResourceView* m_pd3dSRV[2];

	CAnimator();

	~CAnimator();

	void LoadAnimator(const char* pchFilepath);

	void LoadAnimationTexture(ID3D11Device* pcDevice, const wchar_t * pcFilepathA, const wchar_t * pcFilepathB);
};

struct TAnimationBuffer
{
	XMMATRIX m_tBindpose[MAX_JOINTS];
	float	 fCurrentTime;
	float	 fTotalTime;

	int		 nJointCount;
	int		 nCurrentAnimation;
	int		 nFrameCount;
};

class CAnimatorManager
{
private:
	TAnimationBuffer	m_tMatrixBuffer;

	CAnimator**	m_pcAnimators;

public:
	CAnimatorManager(ID3D11Device* device);

	TAnimationBuffer* GetAnimationData(CAnimatorComponent* pcAnimComponent);

	ID3D11ShaderResourceView*const* GetAnimationTexture(int nAnimator);

	~CAnimatorManager();
};

