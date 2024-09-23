#include "R3GameState.h"
#include "R3ExperienceManagerComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3GameState)

AR3GameState::AR3GameState(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ExperienceManagerComponent = CreateDefaultSubobject<UR3ExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
