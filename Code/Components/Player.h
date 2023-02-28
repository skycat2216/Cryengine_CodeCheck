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

namespace primitives
{
	struct capsule;
}

////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////

enum class EPlayerState
{
	Walking, 
	Canter, 
	Sprinting
};

enum class EPlayerStance
{
	Ground,
	Crouch,
	Standing
};

//Deceased

class CPlayerComponent final : public IEntityComponent
{
public:
	CPlayerComponent();
	virtual ~CPlayerComponent() override {};

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
		desc.AddMember(&CPlayerComponent::fWalkSpeed, 'pms', "playerwalkspeed", "Player Walk Speed", "Set walk speed of player", DEFAULT_SPEED_WALKING);
		desc.AddMember(&CPlayerComponent::fCanterSpeed, 'fcs', "playercanterspeed", "Player Canter Speed", "Set canter speed of player", DEFAULT_SPEED_CANTER);
		desc.AddMember(&CPlayerComponent::fSprintSpeed, 'frs', "playersprintspeed", "Player Run Speed", "Set Move Speed Of Player", DEFAULT_SPEED_RUNNING);
		desc.AddMember(&CPlayerComponent::fRotationSpeed,'pros', "playerrotationspeed", "Player Rotation Speed", "Sets the speed of the players rotation", DEFAULT_ROTATION_SPEED);
		desc.AddMember(&CPlayerComponent::fJumpHeight, 'fjh', "playerjumphheight", "Player Jump Height", "Sets the jump height (or initial velocity?) of player", DEFAULT_JUMP_ENERGY);
		desc.AddMember(&CPlayerComponent::iMaxJump, 'fmj', "playermaxjump", "Player Max Jump Count", "Sets how many times player can jump", DEFAULT_JUMP_FREQUENCY);

		desc.AddMember(&CPlayerComponent::vec3CameraStandingPos, 'csp', "camerastandingpos", "Camera Standing Position", "Sets standing camera default position", ZERO);
		desc.AddMember(&CPlayerComponent::vec3CameraCrouchPos, 'ccp', "cameracrouchpos", "Camera Crouch Position", "Sets crouch camera default position", ZERO);
		desc.AddMember(&CPlayerComponent::fRotationLimitsMaxPitch, 'cpm', "camerapitchmax", "Camera Pitch Max", "Maximum rotation value for camera pitch", 1.5f);
		desc.AddMember(&CPlayerComponent::fRotationLimitsMinPitch, 'cpmi', "camerapitchmin", "Camera Pitch Min", "Minimum rotation value for camera pitch", -0.85f);

		desc.AddMember(&CPlayerComponent::fCapsuleHeightCrouch, 'capc', "capsuleheightcrouching", "Capsule Crouching Height", "Height of collision capsule while crouching", DEFAULT_CAPSULE_HEIGHT_CROUCHING);
		desc.AddMember(&CPlayerComponent::fCapsuleHeightStanding, 'caps', "capsuleheightstanding", "Capsule Standing Height", "Height of collision capsule while standing", DEFAULT_CAPSULE_HEIGHT_STANDING);
		desc.AddMember(&CPlayerComponent::fCapsuleGroundOffset, 'capo', "capsulegroundoffset", "Capsule Ground Offset", "Offset of the capsule from the entity floor", DEFAULT_CAPSULE_GROUND_OFFSET);

		desc.AddMember(&CPlayerComponent::fCamOffsetTPSForward, 'cof', "cameraoffsetforward", "Camera Offset Forward", "IDK#1",ZERO);
		desc.AddMember(&CPlayerComponent::fCamOffsetTPSUp, 'cou', "cameraoffsetup", "Camera Offset Upward", "IDK#2", ZERO);
		desc.AddMember(&CPlayerComponent::fCamOffsetFPSForward, 'cof', "cameraoffsetforward", "Camera Offset Forward", "IDK#2", ZERO);
		desc.AddMember(&CPlayerComponent::fCamOffsetFPSUp, 'cou', "cameraoffsetup", "Camera Offset Upward", "IDK#2", ZERO);

	}

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;


protected:
	void Reset();
	void InitializeInput();

	void RecenterCollider();
	void UpdateMovement();
	void UpdateRotation();
	void UpdateCamera(float fFrametime);
	void TryUpdateStance();
	bool IsCapsuleIntersectingGeometry(const primitives::capsule& capsule) const;

private:
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent;
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController;
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent;

	static constexpr EPlayerState DEFAULT_STATE = EPlayerState::Walking;
	static constexpr float DEFAULT_SPEED_WALKING = 3;
	static constexpr float DEFAULT_SPEED_CANTER = 6;
	static constexpr float DEFAULT_SPEED_RUNNING = 9;
	static constexpr float DEFAULT_JUMP_ENERGY = 3;
	static constexpr float DEFAULT_JUMP_FREQUENCY = 2;
	static constexpr float DEFAULT_CAPSULE_HEIGHT_CROUCHING = 0.8;
	static constexpr float DEFAULT_CAPSULE_HEIGHT_STANDING = 1.6;
	static constexpr float DEFAULT_CAPSULE_GROUND_OFFSET = 0.2;

	static constexpr EPlayerStance DEFAULT_STANCE = EPlayerStance::Standing;

	static constexpr float DEFAULT_CAMERA_HEIGHT_STANDING = 2.2;
	static constexpr float DEFAULT_CAMERA_HEIGHT_CROUCH = 3.0;

	static constexpr float DEFAULT_ROTATION_SPEED = 0.002;
	static constexpr float DEFAULT_ROT_LIMIT_PITCH_MIN = -0.9;
	static constexpr float DEFAULT_ROT_LIMIT_PITCH_MAX = 1.15;


	

	//Vars of camera 
	Vec3 vec3CameraStandingPos;
	Vec3 vec3CameraCrouchPos;
	Vec3 vec3CameraFinalPos;

	Vec3 vec3CamEndOffset;
	Quat quatCurrentYaw;
	float fCurrentPitch;

	float fCamOffsetTPSForward;
	float fCamOffsetTPSUp;
	float fCamOffsetFPSForward;
	float fCamOffsetFPSUp;
	bool is_FPS = false;

	Quat quatLookOrientation;


	//Vars of Player
	EPlayerState m_currentPlayerState;

	Vec2 vec2MovementDelta;
	float fMovementSpeed;
	float fCrouchSpeed;
	float fWalkSpeed;
	float fCanterSpeed;
	float fSprintSpeed;

	float fJumpHeight;
	int iMaxJump;
	int iJumpCount;

	EPlayerStance epsCurrentStance;
	EPlayerStance epsDesireStance;
	float fCapsuleHeightStanding;
	float fCapsuleHeightCrouch;
	float fCapsuleGroundOffset;


	//Vars of mouse rotation

	Vec2 vec2MouseDeltaRotation;
	float fRotationSpeed;
	float fRotationLimitsMinPitch;
	float fRotationLimitsMaxPitch;
	

	
};