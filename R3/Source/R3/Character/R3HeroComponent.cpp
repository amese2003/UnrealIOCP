#include "R3HeroComponent.h"
#include "R3PawnExtensionComponent.h"
#include "EnhancedInputSubsystems.h"
#include "R3/R3LogChannel.h"
#include "R3/R3GameplayTags.h"
#include "R3Character.h"
#include "R3/Player/R3PlayerState.h"
#include "R3/Player/R3PlayerController.h"
#include "R3/Character/R3PawnData.h"
#include "R3/Camera/R3CameraComponent.h"
#include "R3/Input/R3MappableConfigPair.h"
#include "R3/Input/R3InputComponent.h"
#include "R3/Character/R3Character.h"
#include "Components/GameFrameworkComponentManager.h"
#include <Kismet/KismetMathLibrary.h>
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3HeroComponent)

/** FeatureName 정의 */
const FName UR3HeroComponent::NAME_ActorFeatureName("Hero");

/** InputConfig의 GameFeatureAction 활성화 ExtensionEvent  이름 */
const FName UR3HeroComponent::NAME_BindInputsNow("BindInputsNow");


UR3HeroComponent::UR3HeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UR3HeroComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 액터에 등록됐는지 확인
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogR3, Error, TEXT("this component has been added to a BP whose base class in not a Pawn!"));
			return;
		}
	}

	RegisterInitStateFeature();
}

void UR3HeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// PawnExtensionComponent에 대해서 (PawnExtension Feature) OnActorInitStateChanged() 관장하도록 (Observing)
	BindOnActorInitStateChanged(UR3PawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);


	// InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FR3GameplayTags::Get().InitState_Spawned));

	// ForceUpdate 진행
	CheckDefaultInitialization();
}

void UR3HeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UR3HeroComponent::CheckDefaultInitialization()
{
	// 앞서 BindOnActorInitStateChanged에서 봤듯이, Hero Feature는 Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers 호출하지 않음.


	// ContinueinitStateChain은 앞서 pawnExtComponent와 같음
	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}
PRAGMA_DISABLE_OPTIMIZATION
void UR3HeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();
	if (Params.FeatureName == UR3PawnExtensionComponent::NAME_ActorFeatureName)
	{
		// R3PawnExtensionComponent의 DataInitialized 상태 변화 관찰 후, UR3HeroComponent도 DataInitialized 상태로 변경
		// - CanChangeInitState 확인

		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION
bool UR3HeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AR3PlayerState* playerState = GetPlayerState<AR3PlayerState>();

	// Spawned 초기화
	if (CurrentState.IsValid() == false && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}


	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!playerState)
		{
			return false;
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// PawnExtensionComponent가 DataInitialized할 때까지 기다림 ( 모든 Feature Component가 DataAvailable인 상태)
		return playerState && Manager->HasFeatureReachedInitState(Pawn, UR3PawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UR3HeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();

	// DataAvailable -> DataInitialized  단계
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AR3PlayerState* clonePS = GetPlayerState<AR3PlayerState>();
		if (!ensure(Pawn && clonePS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UR3PawnData* PawnData = nullptr;

		if (UR3PawnExtensionComponent* PawnExtComp = UR3PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UR3PawnData>();

		}


		if (bIsLocallyControlled && PawnData)
		{
			// 현재 R3Character에 Attach한 CameraComponent를 찾음
			if (UR3CameraComponent* CameraComponent = UR3CameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		if (AR3PlayerController* pc = GetController<AR3PlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

TSubclassOf<UR3CameraMode> UR3HeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UR3PawnExtensionComponent* PawnExtComp = UR3PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UR3PawnData* PawnData = PawnExtComp->GetPawnData<UR3PawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

PRAGMA_DISABLE_OPTIMIZATION
void UR3HeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	// LocalPlayer 가져오기
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem 가져오기위해
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* SubSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(SubSystem);

	// EnhancedInputLocalPlayerSubsystem에 MappingContext를 비워준다
	SubSystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData -> InputConfig 존재 유무 판단
	if (const UR3PawnExtensionComponent* PawnExtComp = UR3PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UR3PawnData* PawnData = PawnExtComp->GetPawnData<UR3PawnData>())
		{
			if (const UR3InputConfig* InputConfig = PawnData->InputConfig)
			{
				const FR3GameplayTags& GameplayTags = FR3GameplayTags::Get();

				//// HeroComponent 가지고 있는 Input Mapping Context를 순회하며, EnhancedInputLocalPlayerSubsystem에 추가해준다
				//for (const FR3MappableConfigPair& Pair : DefaultInputConfigs)
				//{
				//	if (Pair.bShouldActivateAutomatically)
				//	{
				//		FModifyContextOptions Options = {};
				//		Options.bIgnoreAllPressedKeysUntilRelease = false;

				//		// 내부적으로 InputMappingContext에 추가한다
				//		//	- AddPlayerMappableConfig를 간단히 봐야됨
				//		SubSystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
				//	}
				//}

				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.LoadSynchronous())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = SubSystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}

							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							SubSystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				UR3InputComponent* IC = CastChecked<UR3InputComponent>(PlayerInputComponent);
				{
					// InputTag_Move와 InputTag_Look_Mouse에 각각 Input_Move()와 Input_LookMouse() 멤버 변수에 바인딩
					//	- 바인딩 이후 Input 이벤트에 미리 멤버함수 트리거
					{
						TArray<uint32> BindHandles;
						IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::InputAbilityInputTagReleased, BindHandles);
					}
					IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);;
					IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				}
			}
		}
	}

	// GameFeatureAction_AddInputConfig의 HandlePawnExtension 콜백 함수 지정
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);

}
void UR3HeroComponent::AddAdditionalInputConfig(const UR3InputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UR3PawnExtensionComponent* PawnExtComp = UR3PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UR3InputComponent* R3IC = Pawn->FindComponentByClass<UR3InputComponent>();
		if (ensureMsgf(R3IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
		{
			//R3IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}
void UR3HeroComponent::RemoveAdditionalInputConfig(const UR3InputConfig* InputConfig)
{
}
bool UR3HeroComponent::IsReadyToBindInputs() const
{
	return false;
}
PRAGMA_ENABLE_OPTIMIZATION

void UR3HeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AR3PlayerController* Controller = Pawn ? Cast<AR3PlayerController>(Pawn->GetController()) : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		Controller->SetMovementInput(Value);

		if (Value.X != 0.f)
		{
			// Left/Right -> X 값에 들어감
			// MovementDirection은 현재 카메라의 RightVector를 의미함 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovement를 한번 보자
			//	- 내부적으로 MovementDirection * Value.X를 MovementComponent에 적용(더하기) 해준다
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.f) // 앞서 우리는 Foward 적용을 위해 swizzle input modifier를 사용했다.
		{
			// 앞의 Left/Right와 마찬가지로, Foward/BackWard를 적용한다
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}

		// Cache
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);

			const FVector Location = Pawn->GetActorLocation();
			FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, MovementDirection);
			Controller->SetYaw(Rotator.Yaw);
		}
	}
}

void UR3HeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// x에는 Yaw 값
		// - Camera에 대해 Yaw값 사용
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// y에는 Pitch 값
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}

void UR3HeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
}

void UR3HeroComponent::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
}

void UR3HeroComponent::Input_SocketMove(float deltaTime, FVector2d Value)
{
	AR3Character* Pawn = GetPawn<AR3Character>();
	FVector PawnLocation = Pawn->GetActorLocation();

	const FRotator MovementRotation(0.f, Pawn->GetPosInfo()->yaw(), 0.f);

	const FVector NewLocation(Pawn->GetPosInfo()->x(), Pawn->GetPosInfo()->y(), Pawn->GetPosInfo()->z());

	FVector Direction = (NewLocation - PawnLocation);
	float DistanceQuad = Direction.SizeSquared();
	Direction = Direction.GetSafeNormal();

	FVector desireLocation = PawnLocation + (Direction * Pawn->GetCharacterMovement()->GetMaxSpeed() * deltaTime);

	if (DistanceQuad > 9.f)
		Pawn->SetActorLocation(desireLocation);
	else
		Pawn->SetActorLocation(NewLocation);

	Pawn->SetActorRotation(MovementRotation);

	const Protocol::MoveState State = Pawn->GetDestMoveState();


	if (State == Protocol::MOVE_STATE_RUN)
	{
		/*FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);

		MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);*/

		//Pawn->SetActorRotation(MovementRotation);
		//Pawn->AddMovementInput(Pawn->GetActorForwardVector());
		
	}
	else
	{
	}
}
