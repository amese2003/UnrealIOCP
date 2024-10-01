#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "R3/GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "R3HeroComponent.generated.h"

// 전방선언
class UR3CameraMode;
template<class TClass> class TSubclassOf;
struct FR3MappableConfigPair;
struct FInputActionValue;
class UInputComponent;
class UR3InputConfig;

/**
*	component that sets up input and camera handling for player controlled pawns (or bots that simulate player)
*	 - this depends on a PawnExtensionComponent to coordinate initalization
*
*	카메라, 업적 등 플레이어가 제어하는 시스템의 초기화를 처리하는 컴포넌트
*
*/
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class UR3HeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UR3HeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	/** FeatureName 정의 */
	static const FName NAME_ActorFeatureName;

	/** Extension Event 이름 정의 */
	static const FName NAME_BindInputsNow;

	/**
	*	UPawnComponent Interface
	*/
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/**
	* IGameFrameworkInitStateInterface
	*/
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
	virtual void CheckDefaultInitialization() final;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;


	/**
	*	Member methods
	*/
	TSubclassOf<UR3CameraMode> DetermineCameraMode() const;
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void AddAdditionalInputConfig(const UR3InputConfig* InputConfig);
	void RemoveAdditionalInputConfig(const UR3InputConfig* InputConfig);
	bool IsReadyToBindInputs() const;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_MoveEnd(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

	void Input_SocketMove(float deltaTime);
	/**
	*	Member Variable
	*/
	UPROPERTY(EditAnywhere)
		TArray<FR3MappableConfigPair> DefaultInputConfigs;

	UPROPERTY(EditAnywhere)
		TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	bool bReadyToBindInputs;
	

};