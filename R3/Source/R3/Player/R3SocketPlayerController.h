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

	virtual void UpdateSocketMovement(float DeltaTime) override;


	virtual void UpdateMoving(float deltaTime) override;

	/**
	*  Member Methods
	*/


	/**
	* ProtoBuf
	*/
};