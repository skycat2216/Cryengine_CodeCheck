// Copyright 2016-2020 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Player.h"
#include "GamePlugin.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>



namespace
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}

void CPlayerComponent::Initialize()
{
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
	m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
	m_pAdvancedAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();
}
void CPlayerComponent::InitializeInput() 
{

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int ActivatonMode, float value ){vec2MovementDelta.y = value; });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);


	m_pInputComponent->RegisterAction("player", "movebackward", [this](int activatonMode, float value){vec2MovementDelta.y = -value; });
	m_pInputComponent->BindAction("player", "movebackward", eAID_KeyboardMouse, eKI_S);


	m_pInputComponent->RegisterAction("player", "moveright", [this](int activatonMode, float value){vec2MovementDelta.x = value; });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);


	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activatonMode, float value) {vec2MovementDelta.x = -value; });
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);


	//old
	/*
	m_pInputComponent->RegisterAction("player", "jump", [this](int iActivationMode, float value) 
	{
			vec2MovementDelta.z = value; 
	});
	m_pInputComponent->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);


	m_pInputComponent->RegisterAction("player", "crouch", [this](int iActivationMode, float value) 
	{
		vec2MovementDelta.z = -value; 
	});
	m_pInputComponent->BindAction("player", "crouch", eAID_KeyboardMouse, eKI_LShift);
	*/

	m_pInputComponent->RegisterAction("player", "yaw", [this](int activatonMode, float value) {vec2MouseDeltaRotation.x = value; });
	m_pInputComponent->BindAction("player", "yaw", eAID_KeyboardMouse, eKI_MouseY);


	m_pInputComponent->RegisterAction("player", "pitch", [this](int iActivatonMode, float value) {vec2MouseDeltaRotation.y = -value; });
	m_pInputComponent->BindAction("player", "pitch", eAID_KeyboardMouse, eKI_MouseX);




	/*
	m_pInputComponent->RegisterAction("player", "camswitch", [this](int iActivationMode, float value)
	{
			if (iActivationMode == eIS_Pressed)
			{
				CryLog("CamChange Detected");
			}
	});
	m_pInputComponent->BindAction("player", "camswitch", eAID_KeyboardMouse, eKI_F2);
	*/
}

void CPlayerComponent::Playermovement()
{
	Vec3 velocity = Vec3(vec2MovementDelta.x,vec2MovementDelta.y,0.0f);
	velocity.Normalize();
	m_pCharacterController->SetVelocity(m_pEntity->GetWorldRotation() * velocity * fMovementSpeed);
}

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Update;
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		/*
		case Cry::Entity::EEvent::GameplayStarted:
		{
			Initialize();
			InitializeInput();
			break;
		}
		*/

		case Cry::Entity::EEvent::Update:
		{
			//m_pEntity->SetPos(m_pEntity->GetWorldPos() + Vec3(vec2MovementDelta.x, vec2MovementDelta.y, 0.0f));
			m_pCharacterController->SetVelocity(Vec3(vec2MovementDelta.x,vec2MovementDelta.y,0.0f));
















			//�U���O�A��۩x���r�оǨ��|�J��BadCameraPos����],�ݭn�ץ�
			/*
			Ang3 rotationAngle = CCamera::CreateAnglesYPR(Matrix33(quatLookOrientation));
			rotationAngle.x += vec2MouseDeltaRotation.x * fRotationSpeed;
			rotationAngle.y = CLAMP(rotationAngle.y + vec2MouseDeltaRotation.y * fRotationSpeed, fRotationLimitsMinPitch, fRotationLimitsMaxPitch);
			rotationAngle.z = 0;
			quatLookOrientation = Quat(CCamera::CreateOrientationYPR(rotationAngle));

			Ang3 yawAngle = CCamera::CreateAnglesYPR(Matrix33(quatLookOrientation));
			yawAngle.y = 0;
			const Quat finalYaw = Quat(CCamera::CreateOrientationYPR(yawAngle));
			m_pEntity->SetRotation(finalYaw);

			Ang3 pitchAngle = CCamera::CreateAnglesYPR(Matrix33(quatLookOrientation));
			pitchAngle.x = 0;
			Matrix34 finalCamMatrix;
			finalCamMatrix.SetTranslation(m_pCameraComponent->GetTransformMatrix().GetTranslation());
			Matrix33 camRotation = CCamera::CreateOrientationYPR(pitchAngle);
			finalCamMatrix.SetRotation33(camRotation);
			m_pCameraComponent->SetTransformMatrix(finalCamMatrix);
			*/
			}break;
		case Cry::Entity::EEvent::Reset:
		{
			vec2MovementDelta = ZERO;
			vec2MouseDeltaRotation = ZERO;
			quatLookOrientation = IDENTITY;

			Matrix34 camDefaultMatrix;
			camDefaultMatrix.SetTranslation(vec3CameraDefaultPos);
			camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
			m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
			break;
		}
		
	}
}
