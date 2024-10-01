#pragma once

#include "R3BaseController.h"
#include "R3SocketPlayerController.generated.h"

class AR3PlayerState;

UCLASS()
class AR3SocketPlayerController : public AR3BaseController
{
	GENERATED_BODY()

public:
	AR3SocketPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void PlayerTick(float DeltaTime) override;

	/**
	*  Member Methods
	*/

	void UpdateSocketMovement(float DeltaTime);


	/**
	* ProtoBuf
	*/
};