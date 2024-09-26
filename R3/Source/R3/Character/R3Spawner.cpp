#include "R3Spawner.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "R3PawnExtensionComponent.h"
#include "R3/Camera/R3CameraComponent.h"

#include "R3/GameModes/R3MultCharCreateionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R3Spawner)

AR3Spawner::AR3Spawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AR3Spawner::RegisterMonsterActor(int object_id, AActor* MonsterActor)
{
	if (AGameStateBase* GameState = Cast<AGameStateBase>(GWorld->GetGameState()))
	{
		if (UR3MultCharCreateionComponent* Spawner = GameState->FindComponentByClass<UR3MultCharCreateionComponent>())
		{
			Spawner->RegisterMonster(object_id, MonsterActor);
		}
	}
}
