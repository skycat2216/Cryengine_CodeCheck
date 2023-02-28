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

CPlayerComponent::CPlayerComponent()
    : m_pCameraComponent(nullptr)
    , m_pInputComponent(nullptr)
    , m_pAdvancedAnimationComponent(nullptr)
    , m_pCharacterController(nullptr)

    , vec2MouseDeltaRotation(ZERO)
    , vec3CameraStandingPos(Vec3(0.f, 0.f, DEFAULT_CAMERA_HEIGHT_STANDING))
    , vec3CameraCrouchPos(Vec3(0.f, 0.f, DEFAULT_CAMERA_HEIGHT_CROUCH))
    , vec3CamEndOffset(Vec3(0.f,0.f,DEFAULT_CAMERA_HEIGHT_STANDING))
    , quatCurrentYaw(IDENTITY)
    , fCurrentPitch(0.f)

    , m_currentPlayerState(DEFAULT_STATE)
    , fMovementSpeed(ZERO)
    , fWalkSpeed(DEFAULT_SPEED_WALKING)
    , fSprintSpeed(DEFAULT_SPEED_RUNNING)
    , fJumpHeight(DEFAULT_JUMP_ENERGY)    
    , epsCurrentStance(DEFAULT_STANCE)
    , epsDesireStance(DEFAULT_STANCE)
    , fCapsuleHeightStanding(DEFAULT_CAPSULE_HEIGHT_STANDING)
    , fCapsuleHeightCrouch(DEFAULT_CAPSULE_HEIGHT_CROUCHING)
    , fCapsuleGroundOffset(DEFAULT_CAPSULE_GROUND_OFFSET)

    , fRotationSpeed(DEFAULT_ROTATION_SPEED)
    , fRotationLimitsMinPitch(DEFAULT_ROT_LIMIT_PITCH_MIN)
    , fRotationLimitsMaxPitch(DEFAULT_ROT_LIMIT_PITCH_MAX)
    {
    }



void CPlayerComponent::Initialize()
{
    m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
    m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
    m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
    m_pAdvancedAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();



    Reset();
}

void CPlayerComponent::InitializeInput()
{

    m_pInputComponent->RegisterAction("player", "moveforward", [this](int activatonMode, float value) {vec2MovementDelta.y = value; });
    m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);


    m_pInputComponent->RegisterAction("player", "movebackward", [this](int activatonMode, float value) {vec2MovementDelta.y = -value; });
    m_pInputComponent->BindAction("player", "movebackward", eAID_KeyboardMouse, eKI_S);


    m_pInputComponent->RegisterAction("player", "moveright", [this](int activatonMode, float value) {vec2MovementDelta.x = value; });
    m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);


    m_pInputComponent->RegisterAction("player", "moveleft", [this](int activatonMode, float value) {vec2MovementDelta.x = -value; });
    m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);


    m_pInputComponent->RegisterAction("player", "sprint", [this](int activationMode, float value)
        {
            if (activationMode == (int)eAAM_OnPress)
            {
                m_currentPlayerState = EPlayerState::Sprinting;
            }
            else if (activationMode == eAAM_OnRelease)
            {
                m_currentPlayerState = EPlayerState::Walking;
            }
        });
    m_pInputComponent->BindAction("player", "sprint", eAID_KeyboardMouse, eKI_LCtrl);


    m_pInputComponent->RegisterAction("player", "canter", [this](int activationMode, float value)
        {
            if (activationMode == (int)eAAM_OnPress)
            {
                m_currentPlayerState = EPlayerState::Canter;
            }
            else if (activationMode == eAAM_OnRelease)
            {
                m_currentPlayerState = EPlayerState::Walking;
            }
        });
    m_pInputComponent->BindAction("player", "canter", eAID_KeyboardMouse, eKI_LAlt);


    m_pInputComponent->RegisterAction("player", "jump", [this](int activationMode, float value)
    {
        if (m_pCharacterController->IsOnGround())
        {
                iJumpCount = 0;
        }
        if (activationMode == (int)eAAM_OnPress && iJumpCount < iMaxJump)
        {
            CryLog("A Jump Call");
            m_pCharacterController->AddVelocity(Vec3(0, 0, fJumpHeight));
            iJumpCount++;
        }
    });
    m_pInputComponent->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);


    m_pInputComponent->RegisterAction("player", "crouch", [this](int activationMode, float value)
    {
        if (activationMode == (int)eAAM_OnPress)
        {
            epsDesireStance = EPlayerStance::Crouch;
            CryLog("Crouch pressed");
        }
        else if (activationMode == (int)eAAM_OnRelease)
        {
            epsDesireStance = EPlayerStance::Standing;
        }
    });
    m_pInputComponent->BindAction("player", "crouch", eAID_KeyboardMouse, eKI_LShift);


    m_pInputComponent->RegisterAction("player", "yaw", [this](int activatonMode, float value) {vec2MouseDeltaRotation.x = -value; });
    m_pInputComponent->BindAction("player", "yaw", eAID_KeyboardMouse, eKI_MouseX);


    m_pInputComponent->RegisterAction("player", "pitch", [this](int iActivatonMode, float value) {vec2MouseDeltaRotation.y = -value; });
    m_pInputComponent->BindAction("player", "pitch", eAID_KeyboardMouse, eKI_MouseY);

    m_pInputComponent->RegisterAction("player", "camswitch", [this](int iActivationMode, float value)
        {
            /*
            if (iActivationMode == eIS_Pressed)
            {

                if (is_FPS)
                {
                    if(playerState == ePS_Crouching ||)
                        CryLog("CamChange to FPS Detected");
                    is_FPS = false;
                }
            }
            else
            {
                CryLog("CamChange to TPS Detected");
                is_FPS = true;
            }
            */
        });
    m_pInputComponent->BindAction("player", "camswitch", eAID_KeyboardMouse, eKI_F2);

}

void CPlayerComponent::Reset()
{
    vec2MovementDelta = ZERO;
    vec2MouseDeltaRotation = ZERO;

    InitializeInput();

    m_currentPlayerState = EPlayerState::Walking;
    epsCurrentStance = EPlayerStance::Standing;
    epsDesireStance = epsCurrentStance;


    quatCurrentYaw = Quat::CreateRotationZ(m_pEntity->GetWorldRotation().GetRotZ());
    fCurrentPitch = 0.f;

    vec3CamEndOffset = vec3CameraStandingPos;

}



void CPlayerComponent::RecenterCollider()
{
    static bool skip = false;
    if (skip)
    {
        skip = false;
        return;
    }

    auto pChacterController = m_pEntity->GetComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
    if (pChacterController == nullptr)
    {
        return;
    }

    const auto& physParams = m_pCharacterController->GetPhysicsParameters();
    float heightOffset = physParams.m_height*0.5f;
    if (physParams.m_bCapsule)
    {
        heightOffset = heightOffset * 0.5f + physParams.m_radius * 0.5f;
    }

    m_pCharacterController->SetTransformMatrix(Matrix34((IDENTITY), Vec3(0.f, 0.f, 0.005f + heightOffset)));

    skip = true;

    m_pCharacterController->Physicalize();
}

void CPlayerComponent::UpdateMovement()
{

    Vec3 velocity = Vec3(vec2MovementDelta.x, vec2MovementDelta.y, 0.0f);
    velocity.Normalize();
    fMovementSpeed = m_currentPlayerState == EPlayerState::Sprinting ? fSprintSpeed : fWalkSpeed;
    //m_pEntity->SetPos(m_pEntity->GetWorldPos() + Vec3(vec2MovementDelta.x, vec2MovementDelta.y, 0.0f));
    m_pCharacterController->SetVelocity(m_pEntity->GetWorldRotation() * velocity * fMovementSpeed);
}

void CPlayerComponent::UpdateRotation()
{
    quatCurrentYaw *= Quat::CreateRotationZ(vec2MouseDeltaRotation.x * fRotationSpeed);
    m_pEntity->SetRotation(quatCurrentYaw);
}

void CPlayerComponent::UpdateCamera(float fFrametime)
{
    fCurrentPitch = crymath::clamp(fCurrentPitch + vec2MouseDeltaRotation.y * fRotationSpeed, fRotationLimitsMinPitch, fRotationLimitsMaxPitch);

    Vec3 vec3CurrentCamOffset = m_pCameraComponent->GetTransformMatrix().GetTranslation();
    vec3CurrentCamOffset = Vec3::CreateLerp(vec3CurrentCamOffset, vec3CamEndOffset, 10.0f * fFrametime);

    Matrix34 finalCamMartix;
    finalCamMartix.SetTranslation(vec3CurrentCamOffset);
    finalCamMartix.SetRotation33(Matrix33::CreateRotationX(fCurrentPitch));
    m_pCameraComponent->SetTransformMatrix(finalCamMartix);
}

void CPlayerComponent::TryUpdateStance()
{
    if (epsDesireStance == epsCurrentStance) return;
    IPhysicalEntity* pPhysEnt = m_pEntity->GetPhysicalEntity();
    if (pPhysEnt == nullptr)
    {
        CryLog("EPS Nullptr detected");
        return;
    }

    const float radius = m_pCharacterController->GetPhysicsParameters().m_radius * 0.5f;

    float tusHeight = 0.f;
    Vec3 tusCamOffset = ZERO;

    switch (epsDesireStance)
    {
        case EPlayerStance::Ground:
        {
            CryLog("No Actual Implement");
        }break;

        case EPlayerStance::Crouch:
        {
            tusHeight = fCapsuleHeightCrouch;
            tusCamOffset = vec3CameraCrouchPos;
        }break;

        case EPlayerStance::Standing:
        {
            tusHeight = fCapsuleHeightStanding;
            tusCamOffset = vec3CameraStandingPos;
            CryLog("Standing confrimed");
            primitives::capsule capsol;

            capsol.axis.Set(0, 0, 1);

            capsol.center = m_pEntity->GetWorldPos() + Vec3(0, 0, fCapsuleGroundOffset + radius + tusHeight * 0.5f);
            capsol.r = radius;
            capsol.hh = tusHeight * 0.5f;

            if (IsCapsuleIntersectingGeometry(capsol)) return;


        }break;
    }
    pe_player_dimensions playerDemensions;
    pPhysEnt->GetParams(&playerDemensions);

    playerDemensions.heightCollider = fCapsuleGroundOffset + radius + tusHeight * 0.5f;
    playerDemensions.sizeCollider = Vec3(radius, radius, tusHeight * 0.5f);
    vec3CamEndOffset = tusCamOffset;

    epsCurrentStance = epsDesireStance;

    pPhysEnt->SetParams(&playerDemensions);
}

bool CPlayerComponent::IsCapsuleIntersectingGeometry(const primitives::capsule& capsule) const
{
    IPhysicalEntity* pPhysEnt = m_pEntity->GetPhysicalEntity();

    if (pPhysEnt == nullptr)
        return false;

    IPhysicalWorld::SPWIParams pwiParams;

    pwiParams.itype = capsule.type;
    pwiParams.pprim = &capsule;

    pwiParams.pSkipEnts = &pPhysEnt;
    pwiParams.nSkipEnts = 1;

    intersection_params intersectionParams;
    intersectionParams.bSweepTest = false;
    pwiParams.pip = &intersectionParams;

    const int contactCount = static_cast<int>(gEnv->pPhysicalWorld -> PrimitiveWorldIntersection(pwiParams));
    return contactCount > 0;
}

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
    return Cry::Entity::EEvent::GameplayStarted | Cry::Entity::EEvent::Update | Cry::Entity::EEvent::PhysicalTypeChanged | Cry::Entity::EEvent::Reset;
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
    switch (event.event)
    {

        case Cry::Entity::EEvent::GameplayStarted:
        {
        
            Reset();

        }break;


        case Cry::Entity::EEvent::Update:
        {
            const float fFrametime = event.fParam[0];

            TryUpdateStance();
            UpdateMovement();
            UpdateRotation();
            UpdateCamera(fFrametime);





            /*
            Ang3 rotationAng = CCamera::CreateAnglesYPR(Matrix33(quatLookOrientation));
            rotationAng.x += vec2MouseDeltaRotation.x*fRotationSpeed;
            rotationAng.y = CLAMP(rotationAng.y +  vec2MouseDeltaRotation.y * fRotationSpeed, fRotationLimitsMinPitch, fRotationLimitsMaxPitch);
            rotationAng.z = 0;
            quatLookOrientation = Quat(CCamera::CreateOrientationYPR(rotationAng));
            //m_pEntity->SetRotation(quatLookOrientation);
            

            Ang3 yawAngle = CCamera::CreateAnglesYPR(Matrix33(quatLookOrientation));
            yawAngle.y = 0;
            const Quat finalYaw =Quat( CCamera::CreateOrientationYPR(yawAngle));
            m_pEntity->SetRotation(finalYaw);


            Ang3 pitchAngle = CCamera::CreateAnglesYPR(Matrix33(quatLookOrientation));
            pitchAngle.x = 0;
            Matrix34 finalCamMartix;
            Matrix33 camRotation = CCamera::CreateOrientationYPR(pitchAngle);
            finalCamMartix.SetTranslation(m_pCameraComponent->GetTransformMatrix().GetTranslation());
            finalCamMartix.SetRotation33(camRotation);
            m_pCameraComponent->SetTransformMatrix(finalCamMartix);
            */
            

        }break;
        case Cry::Entity::EEvent::PhysicalTypeChanged:
        {
            RecenterCollider();
        }break;
        case Cry::Entity::EEvent::Reset:
        {
            Reset();
        }break;

    }
}





