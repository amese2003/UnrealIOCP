#pragma once

#include "CommonPlayerController.h"
#include "R3SocketPlayerController.generated.h"

class AR3PlayerState;

UCLASS()
class AR3SocketPlayerController : public ACommonPlayerController
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