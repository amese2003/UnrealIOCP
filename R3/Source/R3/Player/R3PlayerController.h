#pragma once

#include "CommonPlayerController.h"
#include "R3/Network/Protocol.pb.h"
#include "R3PlayerController.generated.h"

class AR3PlayerState;

UCLASS()
class AR3PlayerController : public ACommonPlayerController
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


	/**
	* ProtoBuf
	*/
	void SetMovementInput(const FVector2d& Input) { MovementInput = Input; }
	void SetYaw(float yaw) { YawValue = yaw; }
	void SetControlPlayer() { ControlPlayer = true; }
	FVector2d& GetMovementInput() { return MovementInput; }
private:
	bool ControlPlayer = false;
	FVector2D MovementInput;
	FVector2D LastDesiredInput;
	float YawValue = 0.f;

	const float MOVE_PACKET_SEND_DELAY = 0.2f;
	float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;
};