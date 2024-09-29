#pragma once

#include "CommonPlayerController.h"
#include "R3/Network/Protocol.pb.h"
#include "R3BaseController.h"
#include "R3PlayerController.generated.h"

class AR3PlayerState;

UCLASS()
class AR3PlayerController : public AR3BaseController
{
	GENERATED_BODY()

public:
	AR3PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	/**
	*  PlayerController Interface
	*/
	virtual void PlayerTick(float DeltaTime) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	/**
	*  Member Methods
	*/
	AR3PlayerState* GetR3PlayerState() const;
	void SendMovement(float DeltaTime);
	void UpdateSocketMovement(float DeltaTime);


	
};