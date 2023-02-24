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

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
    return Cry::Entity::EEvent::GameplayStarted | Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
    switch (event.event)
    {

        case Cry::Entity::EEvent::GameplayStarted:
        {
        
            InitializeInput();

        }break;


        case Cry::Entity::EEvent::Update:
        {


            Vec3 velocity = Vec3(vec2MovementDelta.x, vec2MovementDelta.y, 0.0f);
            velocity.Normalize();
            fMovementSpeed = m_currentPlayerState == EPlayerState::Sprinting?fSprintSpeed:fWalkSpeed;
            //m_pEntity->SetPos(m_pEntity->GetWorldPos() + Vec3(vec2MovementDelta.x, vec2MovementDelta.y, 0.0f));
            m_pCharacterController->SetVelocity(m_pEntity->GetWorldRotation() * velocity * fMovementSpeed);


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

            

        }break;
        case Cry::Entity::EEvent::Reset:
        {
            vec2MovementDelta = ZERO;
            vec2MouseDeltaRotation = ZERO;
            quatLookOrientation = IDENTITY;

            
            //I still can't believe it disappar after I hit ctrl+c and ctrl+v the vec3CameraPos in the code below.
            //F
            Matrix34 camDefaultMatrix;
            camDefaultMatrix.SetTranslation(vec3CameraDefaultPos);
            camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
            m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
            break;
            
        }
    }
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





    m_pInputComponent->RegisterAction("player", "jump", [this](int activationMode, float value)
        {

            if (m_pCharacterController->IsOnGround())
            {
                iJumpCount=0;
            }
           if(activationMode == (int)eAAM_OnPress && iJumpCount < iMaxJump)
            {
                CryLog("A Jump Call");
                m_pCharacterController->AddVelocity(Vec3(0, 0, fJumpHeight));
                iJumpCount ++;
            }
        });
    m_pInputComponent->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);

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




