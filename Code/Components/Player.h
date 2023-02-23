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


/*
namespace Cry::DefaultComponents
{
	class CCameraComponent;
	class CInputComponent;
	class CCharacterControllerComponent;
	class CAdvancedAnimationComponent;
}
*/
////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CPlayerComponent final : public IEntityComponent
{
public:
	CPlayerComponent() = default;
	virtual ~CPlayerComponent() = default;

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
		desc.AddMember(&CPlayerComponent::fMovementSpeed, 'pms', "playermovespeed",  "Player Move Speed", "Set Move Speed Of Player", ZERO);
		desc.AddMember(&CPlayerComponent::fRotationSpeed,'pros', "playerrotationspeed", "Player Rotation Speed", "Sets the speed of the players rotation", ZERO);
		desc.AddMember(&CPlayerComponent::vec3CameraDefaultPos, 'cdp', "cameradefaultpos", "Camera Default Position", "Sets camera default position", ZERO);
		desc.AddMember(&CPlayerComponent::fRotationLimitsMaxPitch, 'cpm', "camerapitchmax", "Camera Pitch Max", "Maximum rotation value for camera pitch", 1.5f);
		desc.AddMember(&CPlayerComponent::fRotationLimitsMinPitch, 'cpmi', "camerapitchmin", "Camera Pitch Min", "Minimum rotation value for camera pitch", -0.85f);
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
	void Playermovement();


	float fMovementSpeed;
	float fRotationSpeed;
	float fRotationLimitsMinPitch;
	float fRotationLimitsMaxPitch;

	Quat quatLookOrientation;

	Vec2 vec2MouseDeltaRotation;
	Vec2 vec2MovementDelta;
	Vec3 vec3CameraDefaultPos;
};