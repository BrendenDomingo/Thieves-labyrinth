#include "ComponentManager.h"
#include "InputManager.h"

#include "FireDemonController.h"
#include "CameraController.h"
#include "MageController.h"
#include "ChickenController.h"
#include "SpikeTrapController.h"
#include "PathSurface.h"
#include "PathAgent.h"
#include "VikingController.h"
#include "ArcherController.h"
#include "KnightController.h"
#include "IndicatorController.h"
#include "Rigidbody.h"
#include "AnimatorComponent.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "EnumTypes.h"
#include "ParticleEffect.h"
#include "LightComponent.h"

#include "Stats.h"
#include "Entity.h"
#include "DebugDefines.h"

CComponentManager::CComponentManager()
{
	m_bRenderColliders = false;
	m_bRenderTransforms = false;
	m_pcPlayerController = nullptr;
}

IComponent* CComponentManager::CreateComponent(IEntity* pcOwner, int nType)
{ 
	IComponent* newComponent;

	switch(nType)
	{
		case eComponent::TRANSFORM:
		{
			CTransform* newTransform = new CTransform(pcOwner);
			m_pcTransforms.push_back(newTransform);
			newComponent = newTransform;

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CBoxCollider* newCollider = new CBoxCollider(pcOwner);
			m_pcColliders.push_back((ICollider*)newCollider);
			newComponent = newCollider;

			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			CCapsuleCollider* newCollider = new CCapsuleCollider(pcOwner);
			m_pcColliders.push_back((ICollider*)newCollider);
			newComponent = newCollider;

			break;
		}
		case eComponent::RIGIDBODY:
		{
			CRigidbody* newRigidbody = new CRigidbody(pcOwner);
			m_pcRigidbodies.push_back(newRigidbody);
			newComponent = newRigidbody;

			break;
		}
		case eComponent::MESH_RENDERER:
		{
			CMeshRenderer* newMesh = new CMeshRenderer(pcOwner);

			if(pcOwner->m_nEntityType == eEntity::PARTICLE)
				m_pcParticleMeshes.push_back(newMesh);
			else
				m_pcMeshes.push_back(newMesh);

			newComponent = newMesh;

			break;
		}

		case eComponent::STATS:
		{
			CStats* newStats = new CStats(pcOwner);
			m_pcStats.push_back(newStats);
			newComponent = newStats;

			break;
		}

		case eComponent::FIRE_DEMON_CONTROLLER:
		{
			m_pcPlayerController = new CFireDemonController(pcOwner);
			newComponent = m_pcPlayerController;
			break;
		}

		case eComponent::CAMERA_CONTROLLER:
		{
			CCameraController* newCamController = new CCameraController(pcOwner);
			m_pcCameras.push_back(newCamController);
			newComponent = newCamController;

			break;
		}

		case eComponent::ANIMATOR:
		{
			CAnimatorComponent* newAnimator = new CAnimatorComponent(pcOwner);
			m_pcAnimators.push_back(newAnimator);
			newComponent = newAnimator;

			break;
		}

		case eComponent::MAGE_CONTROLLER:
		{
			CMageController* newMageController = new CMageController(pcOwner);
			newComponent = newMageController;
			m_pcEnemyControllers.push_back(dynamic_cast<IEnemyController*>(newMageController));
			break;
		}

		case eComponent::CHICKEN_CONTROLLER:
		{
			CChickenController* newChickenController = new CChickenController(pcOwner);
			newComponent = newChickenController;
			m_pcEnemyControllers.push_back(dynamic_cast<IEnemyController*>(newChickenController));
			break;
		}

		case eComponent::VIKING_CONTROLLER:
		{
			CVikingController* newVikingController = new CVikingController(pcOwner);
			newComponent = newVikingController;
			m_pcEnemyControllers.push_back(dynamic_cast<IEnemyController*>(newVikingController));
			break;
		}

		case eComponent::ARCHER_CONTROLLER:
		{
			CArcherController* newArcherController = new CArcherController(pcOwner);
			newComponent = newArcherController;
			m_pcEnemyControllers.push_back(dynamic_cast<IEnemyController*>(newArcherController));
			break;
		}

		case eComponent::KNIGHT_CONTROLLER:
		{
			CKnightController* newKnightController = new CKnightController(pcOwner);
			newComponent = newKnightController;
			m_pcEnemyControllers.push_back(dynamic_cast<IEnemyController*>(newKnightController));
			break;
		}

		case eComponent::PROJECTILE_COMPONENT:
		{
			CProjectileComponent* newProjectileComponent = new CProjectileComponent(pcOwner);
			m_pcProjectileComponents.push_back(newProjectileComponent);

			newComponent = newProjectileComponent;
			break;
		}

		case eComponent::INVENTORY:
		{
			CInventory* newInventoryComponent = new CInventory(pcOwner);
			m_pcInventories.push_back(newInventoryComponent);
			newComponent = newInventoryComponent;
			break;
		}

		case eComponent::LIGHT:
		{
			CLightComponent* newLight = new CLightComponent(pcOwner);
			m_pcLights.push_back(newLight);
			newComponent = newLight;

			if(pcOwner->m_pcRoom)
			{
				pcOwner->m_pcRoom->AddLight(newLight);
			}

			break;
		}

		case eComponent::SPIKE_TRAP_COMPONENT:
		{
			CSpikeTrapController* newSpikeController = new CSpikeTrapController(pcOwner);
			m_pcSpikeTrapControllers.push_back(newSpikeController);
			newComponent = newSpikeController;

			break;
		}

		case eComponent::PATH_SURFACE:
		{
			CPathSurface* newPathSurface = new CPathSurface(pcOwner);
			m_pcPathSurfaces.push_back(newPathSurface);
			newComponent = newPathSurface;

			break;
		}

		case eComponent::PATH_AGENT:
		{
			CPathAgent* newPathAgent = new CPathAgent(pcOwner);
			m_pcPathAgents.push_back(newPathAgent);
			newComponent = newPathAgent;

			break;
		}

		case eComponent::PARTICLE_EMITTER:
		{
			CParticleEmitter* newParticleEffect = new CParticleEmitter(pcOwner);
			m_pcParticleEmitter.push_back(newParticleEffect);
			newComponent = newParticleEffect;

			break;
		}

		case eComponent::CLICK_INDICATOR:
		{
			CIndicatorController* newIndicatorController = new CIndicatorController(pcOwner);
			m_pcIndicatorControllers.push_back(newIndicatorController);
			newComponent = newIndicatorController;
			break;
		}

		default:
		{
			newComponent = nullptr;
			break;
		}
	}

	newComponent->m_pcOwner = pcOwner;
	newComponent->m_nComponentType = nType;
	newComponent->SetActiveState(pcOwner->IsActive());

	return newComponent;
}

int CComponentManager::DeleteComponent(int nType, IComponent * pcComponent)
{
	switch(nType)
	{
		case eComponent::TRANSFORM:
		{
			for(unsigned int i = 0; i < m_pcTransforms.size(); i++)
			{
				if(m_pcTransforms[i] == pcComponent)
				{
					m_pcTransforms.erase(m_pcTransforms.begin() + i);
					break;
				}
			}

			delete (CTransform*)pcComponent;
			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			for(unsigned int i = 0; i < m_pcColliders.size(); i++)
			{
				if(m_pcColliders[i] == pcComponent)
				{
					m_pcColliders.erase(m_pcColliders.begin() + i);
					break;
				}
			}

			delete (CBoxCollider*)pcComponent;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			for (unsigned int i = 0; i < m_pcColliders.size(); i++)
			{
				if (m_pcColliders[i] == pcComponent)
				{
					m_pcColliders.erase(m_pcColliders.begin() + i);
					break;
				}
			}

			delete (CCapsuleCollider*)pcComponent;
			break;
		}
		case eComponent::RIGIDBODY:
		{
			for(unsigned int i = 0; i < m_pcRigidbodies.size(); i++)
			{
				if(m_pcRigidbodies[i] == pcComponent)
				{
					m_pcRigidbodies.erase(m_pcRigidbodies.begin() + i);
					break;
				}
			}

			delete (CRigidbody*)pcComponent;
			break;
		}
		case eComponent::MESH_RENDERER:
		{
			if(pcComponent->m_pcOwner->m_nEntityType == eEntity::PARTICLE)
			{
				for(unsigned int i = 0; i < m_pcParticleMeshes.size(); i++)
				{
					if(m_pcParticleMeshes[i] == pcComponent)
					{
						m_pcParticleMeshes.erase(m_pcParticleMeshes.begin() + i);
						break;
					}
				}
			}
			else
			{
				for(unsigned int i = 0; i < m_pcMeshes.size(); i++)
				{
					if(m_pcMeshes[i] == pcComponent)
					{
						m_pcMeshes.erase(m_pcMeshes.begin() + i);
						break;
					}
				}
			}

			delete (CMeshRenderer*)pcComponent;

			break;
		}
		case eComponent::FIRE_DEMON_CONTROLLER:
		{
			delete m_pcPlayerController;
			m_pcPlayerController = nullptr;
			break;
		}

		case eComponent::CAMERA_CONTROLLER:
		{
			for(unsigned int i = 0; i < m_pcCameras.size(); i++)
			{
				if(m_pcCameras[i] == pcComponent)
				{
					m_pcCameras.erase(m_pcCameras.begin() + i);
					break;
				}
			}

			delete (CCameraController*)pcComponent;

			break;
		}

		case eComponent::STATS:
		{
			for(unsigned int i = 0; i < m_pcStats.size(); i++)
			{
				if(m_pcStats[i] == pcComponent)
				{
					m_pcStats.erase(m_pcStats.begin() + i);
					break;
				}
			}

			delete (CStats*)pcComponent;

			break;
		}

		case eComponent::ANIMATOR:
		{
			for(unsigned int i = 0; i < m_pcAnimators.size(); i++)
			{
				if(m_pcAnimators[i] == pcComponent)
				{
					m_pcAnimators.erase(m_pcAnimators.begin() + i);
					break;
				}
			}

			delete (CAnimatorComponent*)pcComponent;

			break;
		}

		case eComponent::MAGE_CONTROLLER:
		{
			for (unsigned int i = 0; i < m_pcEnemyControllers.size(); i++)
			{
				if (m_pcEnemyControllers[i] == pcComponent)
				{
					m_pcEnemyControllers.erase(m_pcEnemyControllers.begin() + i);
					break;
				}
			}

			delete (CMageController*)pcComponent;

			break;
		}

		case eComponent::CHICKEN_CONTROLLER:
		{
			for (unsigned int i = 0; i < m_pcEnemyControllers.size(); i++)
			{
				if (m_pcEnemyControllers[i] == pcComponent)
				{
					m_pcEnemyControllers.erase(m_pcEnemyControllers.begin() + i);
					break;
				}
			}

			delete (CChickenController*)pcComponent;

			break;
		}

		case eComponent::VIKING_CONTROLLER:
		{
			for (unsigned int i = 0; i < m_pcEnemyControllers.size(); i++)
			{
				if (m_pcEnemyControllers[i] == pcComponent)
				{
					m_pcEnemyControllers.erase(m_pcEnemyControllers.begin() + i);
					break;
				}
			}

			delete (CVikingController*)pcComponent;

			break;
		}

		case eComponent::ARCHER_CONTROLLER:
		{
			for (unsigned int i = 0; i < m_pcEnemyControllers.size(); i++)
			{
				if (m_pcEnemyControllers[i] == pcComponent)
				{
					m_pcEnemyControllers.erase(m_pcEnemyControllers.begin() + i);
					break;
				}
			}

			delete (CArcherController*)pcComponent;

			break;
		}

		case eComponent::KNIGHT_CONTROLLER:
		{
			for (unsigned int i = 0; i < m_pcEnemyControllers.size(); i++)
			{
				if (m_pcEnemyControllers[i] == pcComponent)
				{
					m_pcEnemyControllers.erase(m_pcEnemyControllers.begin() + i);
					break;
				}
			}

			delete (CKnightController*)pcComponent;

			break;
		}

		case eComponent::PROJECTILE_COMPONENT:
		{
			for(unsigned int i = 0; i < m_pcProjectileComponents.size(); i++)
			{
				if(m_pcProjectileComponents[i] == pcComponent)
				{
					m_pcProjectileComponents.erase(m_pcProjectileComponents.begin() + i);
					break;
				}
			}

			delete (CProjectileComponent*)pcComponent;
			break;
		}

		case eComponent::INVENTORY:
		{
			for (unsigned int i = 0; i < m_pcInventories.size(); i++)
			{
				if (m_pcInventories[i] == pcComponent)
				{
					m_pcInventories.erase(m_pcInventories.begin() + i);
					break;
				}
			}

			delete (CInventory*)pcComponent;

			break;
		}

		case eComponent::LIGHT:
		{
			for(unsigned int i = 0; i < m_pcLights.size(); i++)
			{
				if(m_pcLights[i] == pcComponent)
				{
					m_pcLights.erase(m_pcLights.begin() + i);
					break;
				}
			}

			if(pcComponent->m_pcOwner->m_pcRoom)
			{
				pcComponent->m_pcOwner->m_pcRoom->RemoveLight((CLightComponent*)pcComponent);
			}

			delete (CLightComponent*)pcComponent;

			break;
		}

		case eComponent::SPIKE_TRAP_COMPONENT:
		{
			for (unsigned int i = 0; i < m_pcSpikeTrapControllers.size(); i++)
			{
				if (m_pcSpikeTrapControllers[i] == pcComponent)
				{
					m_pcSpikeTrapControllers.erase(m_pcSpikeTrapControllers.begin() + i);
					break;
				}
			}

			delete (CSpikeTrapController*)pcComponent;

			break;
		}

		case eComponent::PATH_SURFACE:
		{
			for (unsigned int i = 0; i < m_pcPathSurfaces.size(); i++)
			{
				if (m_pcPathSurfaces[i] == pcComponent)
				{
					m_pcPathSurfaces.erase(m_pcPathSurfaces.begin() + i);
					break;
				}
			}

			delete (CPathSurface*)pcComponent;

			break;
		}

		case eComponent::PATH_AGENT:
		{
			for (unsigned int i = 0; i < m_pcPathAgents.size(); i++)
			{
				if (m_pcPathAgents[i] == pcComponent)
				{
					m_pcPathAgents.erase(m_pcPathAgents.begin() + i);
					break;
				}
			}

			delete (CPathAgent*)pcComponent;

			break;
		}

		case eComponent::PARTICLE_EMITTER:
		{
			for (unsigned int i = 0; i < m_pcParticleEmitter.size(); i++)
			{
				if (m_pcParticleEmitter[i] == pcComponent)
				{
					m_pcParticleEmitter[i]->Disable();

					m_pcParticleEmitter.erase(m_pcParticleEmitter.begin() + i);
					break;
				}
			}

			delete (CParticleEmitter*)pcComponent;

			break;
		}

		case eComponent::CLICK_INDICATOR:
		{
			for(unsigned int i = 0; i < m_pcIndicatorControllers.size(); i++)
			{
				if(m_pcIndicatorControllers[i] == pcComponent)
				{
					m_pcIndicatorControllers.erase(m_pcIndicatorControllers.begin() + i);
					break;
				}
			}

			delete (CIndicatorController*)pcComponent;

			break;
		}


		default:
		{
			return -1;
			break;
		}
	}
	pcComponent = nullptr;

	return 0;
}

int CComponentManager::GetComponentCountOfType(int nType)
{
	switch(nType)
	{
		case eComponent::TRANSFORM:
		{
			return (int)m_pcTransforms.size();
			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			int nResult = 0;
			for (ICollider* collider : m_pcColliders)
			{
				if (collider->m_nComponentType == eComponent::BOX_COLLIDER)
					nResult++;
			}
			return nResult;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			int nResult = 0;
			for (ICollider* collider : m_pcColliders)
			{
				if (collider->m_nComponentType == eComponent::CAPSULE_COLLIDER)
					nResult++;
			}
			return nResult;
			break;
		}

		case eComponent::RIGIDBODY:
		{
			return (int)m_pcRigidbodies.size();
			break;
		}
		case eComponent::MESH_RENDERER:
		{
			return (int)m_pcMeshes.size();
			break;
		}
		case eComponent::FIRE_DEMON_CONTROLLER:
		{
			if(m_pcPlayerController != nullptr)
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;
		}

		case eComponent::CAMERA_CONTROLLER:
		{
			return (int)m_pcCameras.size();
			break;
		}

		case eComponent::PROJECTILE_COMPONENT:
		{
			return (int)m_pcProjectileComponents.size();
			break;
		}

		case eComponent::INVENTORY:
		{
			return (int)m_pcInventories.size();
			break;
		}

		case eComponent::LIGHT:
		{
			return (int)m_pcLights.size();
			break;
		}

		case eComponent::PARTICLE_EMITTER:
		{
			return (int)m_pcParticleEmitter.size();
			break;
		}

		default:
		{
			return -1;
			break;
		}
	}
}

void CComponentManager::UpdateControllers(float fDeltaTime)
{
	if(m_pcPlayerController)
	{
		if(m_pcPlayerController->IsActive())
		{
			m_pcPlayerController->Update(fDeltaTime);
		}
	}

	for ( CCameraController* pcController : m_pcCameras)
	{
		if(pcController->IsActive())
		{
			pcController->Update();
		}
	}

	for(CProjectileComponent* pcProjectile : m_pcProjectileComponents)
	{
		if(pcProjectile->IsActive())
		{
			pcProjectile->Update(fDeltaTime);
		}
	}

	for (CSpikeTrapController* pcSpikes: m_pcSpikeTrapControllers)
	{
		if (pcSpikes->IsActive())
		{
			pcSpikes->Update(fDeltaTime);
		}
	}

	for(CIndicatorController* pcIndicator : m_pcIndicatorControllers)
	{
		if(pcIndicator->IsActive())
		{
			pcIndicator->Update(fDeltaTime);
		}
	}

	for(CParticleEmitter* pcEmitter : m_pcParticleEmitter)
	{
		if(pcEmitter->IsActive() && pcEmitter->IsRunning())
		{
			pcEmitter->Update(fDeltaTime);
		}
		else if(pcEmitter->IsRunning())
		{
			pcEmitter->Disable();
		}

	}

	
#if TRANSFORMS

	if(CInputManager::GetKeyDown('T'))
	{
		m_bRenderTransforms = !m_bRenderTransforms;
	}

	if(m_bRenderTransforms)
	{
		for(CTransform* pcTransform : m_pcTransforms)
		{
			if(pcTransform->IsActive())
			{
				pcTransform->RenderTransform();
			}
		}
	}

#endif


#if COLLIDERS

	if (CInputManager::GetKeyDown('C'))
	{
		m_bRenderColliders = !m_bRenderColliders;
	}

	if(m_bRenderColliders)
	{
		for(ICollider* pcCollider : m_pcColliders)
		{
			if(pcCollider->IsActive())
			{
				pcCollider->RenderCollider();
			}
		}
	}

#endif

}

std::vector<CTransform*>* CComponentManager::GetTransforms()
{
	return &m_pcTransforms;
}

std::vector<ICollider*>* CComponentManager::GetColliders()
{
	return &m_pcColliders;
}

std::vector<CRigidbody*>* CComponentManager::GetRigidbodies()
{
	return &m_pcRigidbodies;
}

std::vector<CMeshRenderer*> CComponentManager::GetMeshes()
{
	return m_pcMeshes;
}


std::vector<IEnemyController*> CComponentManager::GetEnemyControllers()
{
	return m_pcEnemyControllers;
}

std::vector<CParticleEmitter*>* CComponentManager::GetParticleEmitters()
{
	return &m_pcParticleEmitter;
}

std::vector<CPathAgent*> CComponentManager::GetPathAgents()
{
	return m_pcPathAgents;
}

std::vector<CCameraController*> CComponentManager::GetCameraController()
{
	return m_pcCameras;
}

CComponentManager::~CComponentManager()
{
	if(m_pcPlayerController != nullptr)
	{
		DeleteComponent(m_pcPlayerController->m_nComponentType, m_pcPlayerController);
	}

	for(CTransform* pcComponent : m_pcTransforms)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(ICollider* pcComponent : m_pcColliders)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CRigidbody* pcComponent : m_pcRigidbodies)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CMeshRenderer* pcComponent : m_pcMeshes)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CMeshRenderer* pcComponent : m_pcParticleMeshes)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CCameraController* pcComponent : m_pcCameras)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CProjectileComponent* pcComponent : m_pcProjectileComponents)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CInventory* pcComponent : m_pcInventories)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CAnimatorComponent* pcComponent : m_pcAnimators)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (IEnemyController* pcComponent : m_pcEnemyControllers)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CStats* pcComponent : m_pcStats)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CPathSurface* pcComponent : m_pcPathSurfaces)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CPathAgent* pcComponent : m_pcPathAgents)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CParticleEmitter* pcComponent : m_pcParticleEmitter)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}
}
