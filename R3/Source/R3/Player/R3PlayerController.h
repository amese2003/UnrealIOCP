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
	virtual void UpdateSocketMovement(float DeltaTime);
	AR3PlayerState* GetR3PlayerState() const;

	virtual void UpdateIdle();
	virtual void MoveToNextPos(float deltaTime) override;
	virtual void CheckUpdateFlag(float deltaTime) override;

	void SetDirInput(FVector Input);
	FVector GetDirInput();
	void SetMoveKeyPress(bool Input);


private:
	bool bMoveKeyPress = false;
	FVector InputDir = FVector::Zero();

	float MOVE_PACKET_SEND_DELAY = 0.2f;
	float MovePacketSendTimer = 0.2f;
};