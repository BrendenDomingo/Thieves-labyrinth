
#pragma once
#include "Component.h"
#include "Transform.h"
#include "EntityManager.h"
#include "Entity.h"
#include "CameraController.h"
#include <vector>

class IEntity;
class CParticleEntity;
class CTransform;
class CMeshRenderer;
class CRigidbody;
class CEntityManager;

class CParticleEmitter : public IComponent
{
	//Particle Mesh
	int m_nVertices;
	int m_nIndices;
	int m_pnTexture[3]{ -1 };
	int m_nSampler;
	int m_nInputLayout;
	int m_nVS;
	int m_nPS;

	// Emitter 
	int m_nMaxParticles;

	int m_nEmitterShape;

	int m_nParticlesPerEmission;

	float m_fLife;

	float m_nEmissionRate;
	float m_fSpawnTimer;

	bool m_bIsScaling;
	CMath::TVECTOR3 m_vInitialScale;
	CMath::TVECTOR3 m_vFinalScale;

	bool m_bIsMoving;
	float m_fMinSpeed;
	float m_fMaxSpeed;

	float m_fEmissionAngle;
	CMath::TVECTOR3 m_vEmitterDirection;

	bool m_bIsRotating;
	float m_fMaxRotationSpeed;
	float m_fMinRotationSpeed;

	bool m_bIsLooping;
	bool m_bIsPingPong;

	CMath::TVECTOR3 m_vOffset;

	CMath::TVECTOR3 m_vMaxExtent;
	CMath::TVECTOR3 m_vMinExtent;

	float m_fMaxInitialZRot;
	float m_fMinInitialZRot;

	CParticleEntity* m_pcParticleRef;

	CTransform* m_pcCameraTransform;
	CTransform* m_pcEmitterTransform;

	bool m_bIsRunning;

	void InitializeReferenceParticle();
	void ResetParticle(CParticleEntity* pcParticle);


public:
	std::vector<CParticleEntity*> m_pcParticles; //Make it private and add accessor 

	#pragma region Set

	void SetVertexBuffer(int nBuffer);
	void SetIndexBuffer(int nBuffer);

	void SetTexture(int nTexture, int nSlot);

	void SetSampler(int nSampler);
	void SetInputLayout(int nInputLayout);

	void SetVertexShader(int nVS);
	void SetPixelShader(int nPS);

	void SetMaxParticles(int nMaxParticles);

	void SetEmitterShape(int nEmitterShape);

	void SetParticlesPerEmission(int nNumParticles);

	void SetLife(float nLifetime);

	void SetParticleStartScale(CMath::TVECTOR3 vScale);
	void SetParticleEndScale(CMath::TVECTOR3 vScale);

	void SetEmissionRate(float nEmissionRate);

	void SetMaxRotationSpeed(float nRotationSpeed);
	void SetMinRotationSpeed(float nRotationSpeed);

	void SetLoop(bool bLoop);
	void SetPingPong(bool bPingPong);

	void SetOffset(CMath::TVECTOR3 vOffset);

	void SetMaxSpeed(float fSpeed);
	void SetMinSpeed(float fSpeed);

	void SetEmissionAngle(float fAngle);

	void SetMaxExtent(CMath::TVECTOR3 vMaxExtent);
	void SetMinExtent(CMath::TVECTOR3 vMinExtent);

	void SetEmitterDirection(CMath::TVECTOR3 vDirection);

	void SetMaxInitialRotation(float fAngle);

	void SetMinInitialRotation(float fAngle);

	#pragma endregion

	float GetLife();


	void RefreshEmitterSettings();

	CParticleEmitter& operator=(CParticleEmitter& cCopy);

	CParticleEmitter(IEntity* pcOwner);
	~CParticleEmitter();

	void Update(float fDeltaTime);

	void Disable();

	void DisableOnCollision(int nNewTexture);

	bool IsRunning();
};
