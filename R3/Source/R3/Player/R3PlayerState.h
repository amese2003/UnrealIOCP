#pragma once

#include "GameFramework/PlayerState.h"
#include "ModularPlayerState.h"
#include "R3PlayerState.generated.h"


class UR3PawnData;
class UR3ExperienceDefinition;

UCLASS()
class AR3PlayerState : public AModularPlayerState
{
	GENERATED_BODY()

public:
	AR3PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	*	AActor Interface
	*/
	virtual void PostInitializeComponents() final;

	/**
	*	Member methods
	*/
	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const UR3PawnData* InPawnData);
	void OnExperienceLoaded(const UR3ExperienceDefinition* CurrentExperience);



	UPROPERTY()
		TObjectPtr<const UR3PawnData> PawnData;

};