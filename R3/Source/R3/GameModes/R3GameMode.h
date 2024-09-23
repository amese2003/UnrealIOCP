#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ModularGameMode.h"
#include "R3GameMode.generated.h" 


class UR3ExperienceDefinition;
class UR3PawnData;

UCLASS()
class R3_API AR3GameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	AR3GameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	*	AGameModeBase interface
	*/
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) final;
	virtual void InitGameState() final;

	// SpawnDefaultPawnAtTransform
	// BlueprintNativeEvent -> C++에서 구현예정이지만, Blueprint에서 재정의가능
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;
	// HandleStartingNewPlayer
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;
	/** GetDefaultPawnClassForController */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;


	/**
	*	Member method
	*/
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UR3ExperienceDefinition* CurrentExperience);
	const UR3PawnData* GetPawnDataForController(const AController* InController) const;
	void RestartPlayerForController();
};