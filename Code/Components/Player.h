// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <array>
#include <numeric>

#include <CryEntitySystem/IEntityComponent.h>
#include <CryMath/Cry_Camera.h>
#include <ICryMannequin.h>

#include <CrySchematyc/Utils/EnumFlags.h>
#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Audio/ListenerComponent.h>



namespace Cry::DefaultComponents
{
	class CCameraComponent;
	class CInputComponent;
	class CCharacterControllerComponent;
	class CAdvancedAnimationComponent;
}

////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////

enum class EPlayerState
{
	Walking, Canter, Sprinting
};

class CPlayerComponent final : public IEntityComponent
{
public:
	CPlayerComponent() = default;
	virtual ~CPlayerComponent() = default;

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
		desc.AddMember(&CPlayerComponent::fWalkSpeed, 'pms', "playerwalkspeed", "Player Walk Speed", "Set walk speed of player", ZERO);
		desc.AddMember(&CPlayerComponent::fCanterSpeed, 'fcs', "playercanterspeed", "Player Canter Speed", "Set canter speed of player", ZERO);
		desc.AddMember(&CPlayerComponent::fSprintSpeed, 'frs', "playersprintspeed", "Player Run Speed", "Set Move Speed Of Player", ZERO);
		desc.AddMember(&CPlayerComponent::fRotationSpeed,'pros', "playerrotationspeed", "Player Rotation Speed", "Sets the speed of the players rotation", ZERO);
		desc.AddMember(&CPlayerComponent::fJumpHeight, 'fjh', "playerjumphheight", "Player Jump Height", "Sets the jump height (or initial velocity?) of player",1.0f);
		desc.AddMember(&CPlayerComponent::iMaxJump, 'fmj', "playermaxjump", "Player Max Jump Count", "Sets how many times player can jump",1);

		desc.AddMember(&CPlayerComponent::vec3CameraDefaultPos, 'cdp', "cameradefaultpos", "Camera Default Position", "Sets camera default position", ZERO);
		desc.AddMember(&CPlayerComponent::fRotationLimitsMaxPitch, 'cpm', "camerapitchmax", "Camera Pitch Max", "Maximum rotation value for camera pitch", 1.5f);
		desc.AddMember(&CPlayerComponent::fRotationLimitsMinPitch, 'cpmi', "camerapitchmin", "Camera Pitch Min", "Minimum rotation value for camera pitch", -0.85f);

		desc.AddMember(&CPlayerComponent::fCamOffsetTPSForward, 'cof', "cameraoffsetforward", "Camera Offset Forward", "IDK#1",ZERO);
		desc.AddMember(&CPlayerComponent::fCamOffsetTPSUp, 'cou', "cameraoffsetup", "Camera Offset Upward", "IDK#2", ZERO);
		desc.AddMember(&CPlayerComponent::fCamOffsetFPSForward, 'cof', "cameraoffsetforward", "Camera Offset Forward", "IDK#1", ZERO);
		desc.AddMember(&CPlayerComponent::fCamOffsetFPSUp, 'cou', "cameraoffsetup", "Camera Offset Upward", "IDK#2", ZERO);

	}

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

private:
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent;
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController;
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent;

	void InitializeInput();
	void UpdateCameraPos();

	

	Vec2 vec2MouseDeltaRotation;
	Vec2 vec2MovementDelta;
	Vec3 vec3CameraDefaultPos;
	
	Quat quatLookOrientation;

	float fMovementSpeed;
	float fWalkSpeed;
	float fCanterSpeed;
	float fSprintSpeed;
	float fRotationSpeed;
	float fRotationLimitsMinPitch;
	float fRotationLimitsMaxPitch;
	float fJumpHeight;
	int iMaxJump;
	int iJumpCount;


	float fCamOffsetTPSForward;
	float fCamOffsetTPSUp;
	float fCamOffsetFPSForward;
	float fCamOffsetFPSUp;

	EPlayerState m_currentPlayerState;
	
	bool is_FPS = false;
	
};