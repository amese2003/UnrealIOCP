#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "R3PawnExtensionComponent.generated.h"


class UR3PawnData;

UCLASS()
class UR3PawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UR3PawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const FName NAME_ActorFeatureName;
	static UR3PawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor) ? Actor->FindComponentByClass<UR3PawnExtensionComponent>() : nullptr; }

	/**
	*  member Methods
	*/
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const UR3PawnData* InPawnData);
	void SetupPlayerInputComponent();

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
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;

	/**
	*  Pawn을 생성한 데이터 캐싱
	*/
	UPROPERTY(EditInstanceOnly, Category = "R3|Pawn");
	TObjectPtr<const UR3PawnData> PawnData;

};
