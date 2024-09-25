#pragma once


#include "ModularCharacter.h"
#include "R3Spawner.generated.h"


class UR3CameraComponent;

UCLASS()
class R3_API AR3Spawner : public AModularCharacter
{
	GENERATED_BODY()

public:
	AR3Spawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};