#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"

#include "R3MultCharCreateionComponent.generated.h"

class UR3ExperienceDefinition;
class AR3SocketPlayerController;
class ACommonPlayerController;

UCLASS(Blueprintable, Abstract)
class UR3MultCharCreateionComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UR3MultCharCreateionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	*	UActorComponent's Interface
	*/
	virtual void BeginPlay() override;
	void SpawnMultiplayerCharacter(const Protocol::ObjectInfo& Info);
	void RestartPlayer(const Protocol::ObjectInfo& Info);
	bool ContainPlayer(uint64 object_id) { return SpawnedBotList.Contains(object_id); }
	void UpdateCharacterMovement(const Protocol::PosInfo& Info);

private:
	void OnExperienceLoaded(const UR3ExperienceDefinition* Experience);

public:
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TSubclassOf<AR3SocketPlayerController> MultiControllerClass;

	UPROPERTY(Transient)
		TMap<uint64, TObjectPtr<ACommonPlayerController>> SpawnedBotList;


	

};