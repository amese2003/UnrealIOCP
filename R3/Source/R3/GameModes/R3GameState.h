#pragma once

#include "GameFramework/GameStateBase.h"
#include "ModularGameState.h"
#include "R3GameState.generated.h"

class UR3ExperienceManagerComponent;

UCLASS(Config = Game)
class AR3GameState : public AModularGameStateBase
{
	GENERATED_BODY()
public:
	AR3GameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
		TObjectPtr<UR3ExperienceManagerComponent> ExperienceManagerComponent;

	
};