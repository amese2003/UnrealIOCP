#pragma once

#include "CommonPlayerController.h"
#include "R3BaseController.h"
#include "R3MonsterController.generated.h"

class AR3PlayerState;

UCLASS()
class AR3MonsterController : public AR3BaseController
{
	GENERATED_BODY()

public:
	AR3MonsterController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void PlayerTick(float DeltaTime) override;

};