#pragma once


#include "ModularCharacter.h"
#include "R3Spawner.generated.h"


class UR3CameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnMonsterDelegate);

UCLASS()
class R3_API AR3Spawner : public AModularCharacter
{
	GENERATED_BODY()

public:
	AR3Spawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



	UFUNCTION(BlueprintImplementableEvent)
		void SpawnMonster(int object_id, int CreatureID, FVector Location, FRotator Rotator, float speed = 1024.f);

	UFUNCTION(BlueprintCallable)
		void RegisterMonsterActor(int object_id, AActor* MonsterActor);
};