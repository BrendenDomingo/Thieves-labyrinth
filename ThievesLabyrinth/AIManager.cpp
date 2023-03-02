#include "AIManager.h"
#include "Time.h"
#include "Rigidbody.h"
#include "ComponentManager.h"
#include "EventManager.h"
#include "InputManager.h"
#include "PhysicsManager.h"

#include "MageController.h"
#include "ChickenController.h"
#include "KnightController.h"

#include "Transform.h"
#include "PathAgent.h"
#include "PathSurface.h"
#include "Level.h"
#include "EnumTypes.h"
#include "Stats.h"
#include "Entity.h"

CMath::TVECTOR3 CAIManager::GetSeparationAccel(IEnemyController * pcEnemy)
{
	CMath::TVECTOR3 tSum = CMath::TVECTOR3(0, 0, 0);
	CTransform* pcTransform = pcEnemy->m_pcOwner->GetComponent<CTransform>();
	CRigidbody* pcRigidBody = pcEnemy->m_pcOwner->GetComponent<CRigidbody>();

	for (IEnemyController* controller : m_pcEnemyControllers)
	{
		if (controller == pcEnemy)
			continue;

		CTransform* pcOtherTransform = controller->m_pcOwner->GetComponent<CTransform>();
		CRigidbody* pcOtherRigidbody = controller->m_pcOwner->GetComponent<CRigidbody>();

		CMath::TVECTOR3 tDistanceVector = pcTransform->GetPosition() - pcOtherTransform->GetPosition();

		float fDistance = CMath::Vector3Magnitude(tDistanceVector);

		float fSafeDistance = pcEnemy->GetSafeDistance() + controller->GetSafeDistance();

		float fSpeed = CMath::Vector3Magnitude(pcRigidBody->GetVelocity());
		float fOtherSpeed = CMath::Vector3Magnitude(pcRigidBody->GetVelocity());

		float fSumSpeed = fSpeed + fOtherSpeed;
		if (fSumSpeed == 0)
			return CMath::TVECTOR3(0, 0, 0);

		if (fDistance < fSafeDistance)
		{
			CMath::TVECTOR3 tDirectionVector = CMath::Vector3Normalize(tDistanceVector);
			tDirectionVector = tDirectionVector * (fSafeDistance - fDistance) / fSafeDistance;
			tDirectionVector = tDirectionVector * (fSumSpeed - fSpeed) / fSumSpeed;
			tSum = tSum + tDirectionVector;
		}
	}
	if (CMath::Vector3Magnitude(tSum) > 1.0f)
		tSum = CMath::Vector3Normalize(tSum);

	return tSum * m_fSeparationStrength;
}

CAIManager::CAIManager(CComponentManager* pcComponentManager)
{
	m_pcComponentManager = pcComponentManager;
	m_fSeparationStrength = 500.0f;
	m_pcPlayerAgent = nullptr;
	m_bCalculateWeights = false;
	currentRoom = CLevel::m_pcCurrentRoom;
}
CAIManager::~CAIManager()
{
}

void CAIManager::FindPlayerPath()
{
	CheckForAgent();

	m_pcPlayerAgent->Start();
}
void CAIManager::CheckForAgent()
{
	if (!m_pcPlayerAgent)
	{
		TEntityMessage message = TEntityMessage(0);
		m_pcPlayerAgent = ((CPlayerEntity*)CEventManager::SendEntityMessage(message))->GetComponent<CPathAgent>();
	}

	CPathSurface* pcSurface = m_pcPlayerAgent->GetSurface();
	CPathSurface* pcRoomSurface = CLevel::m_pcCurrentRoom->GetComponent<CPathSurface>();

	if (!pcSurface || pcSurface != pcRoomSurface)
	{
		m_pcPlayerAgent->SetSurface(pcRoomSurface);
	}
}

void CAIManager::Reset()
{
	m_pcPlayerAgent = nullptr;
	m_bCalculateWeights = false;
	currentRoom = CLevel::m_pcCurrentRoom;
}

void CAIManager::Update(float fDeltaTime)
{
	CheckForAgent();
	if (m_bCalculateWeights)
	{
		m_pcPlayerAgent->GetSurface()->CalculateWeights();
		m_bCalculateWeights = false;
	}

	if (!m_pcPlayerAgent->GetSurface()->HasWeights())
	{
		m_bCalculateWeights = true;
		return;
	}

	for (CPathAgent* agent : m_pcComponentManager->GetPathAgents())
	{
		if (agent->IsNavigating())
			agent->Navigate();
	}

	if (CInputManager::GetKeyDown('K'))
	{
		CMath::TVECTOR2 vMouse;
		CMath::TVECTOR3 vStart, vEnd, vSegment, tVel, tNormalVelocity, tFloorPosition;
		CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
		CInputManager::GetMouseWorldSpace(vMouse, vStart, vEnd);
		vSegment = vEnd - vStart;

		if (CPhysicsManager::RaycastMouseToFloor(vStart, CMath::Vector3Normalize(vSegment), OUT tFloorPosition))
		{
			CheckForAgent();

			m_pcPlayerAgent->SetTarget(tFloorPosition);
		}
	}
	UpdateEnemyBehavior(fDeltaTime);
	//UpdateFlockingBehavior(fDeltaTime);
}

void CAIManager::UpdateEnemyBehavior(float fDeltaTime)
{
	m_pcEnemyControllers = m_pcComponentManager->GetEnemyControllers();

	for (IEnemyController* controller : m_pcEnemyControllers)
	{
		if (controller->IsActive())
			controller->Update(fDeltaTime);
	}
}

void CAIManager::UpdateFlockingBehavior(float fDeltaTime)
{
	for (IEnemyController* controller : m_pcEnemyControllers)
	{
		CMath::TVECTOR3 acceleration = GetSeparationAccel(controller);
		acceleration = acceleration * controller->m_pcOwner->GetComponent<CStats>()->GetMovementSpeed() * fDeltaTime;
		
		CRigidbody* tRigidbody = controller->m_pcOwner->GetComponent<CRigidbody>();
		CTransform* tTransform = controller->m_pcOwner->GetComponent<CTransform>();

		CMath::TVECTOR3 tVelocity = tRigidbody->GetVelocity();
		tVelocity = tVelocity + acceleration;

		CMath::TVECTOR3 tNormalVelocity = CMath::Vector3Normalize(tVelocity);
		CMath::TVECTOR3 tFinalVelocity = tNormalVelocity * controller->m_pcOwner->GetComponent<CStats>()->GetMovementSpeed();
		tRigidbody->SetVelocity(tVelocity);
		tTransform->TurnTo(tNormalVelocity);
	}
}
