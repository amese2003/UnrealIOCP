#pragma once

#include "CommonPlayerController.h"
#include "R3/Network/Protocol.pb.h"
#include "R3BaseController.generated.h"

class AR3PlayerState;

UCLASS()
class AR3BaseController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	AR3BaseController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/**
	* ProtoBuf
	*/
	virtual void UpdateSocketMovement(float DeltaTime) {}

	virtual void UpdateIdle() {}
	virtual void UpdateMoving(float DeltaTime) {}
	virtual void UpdateSkill() {}
	virtual void UpdateDead() {}


	void InitializePosition();
	void ResetCharacterInfo();

	Protocol::ObjectInfo* GetPlayerInfo() { return CharacterInfo; }
	void SetPlayerInfo(const Protocol::ObjectInfo& Info);
	virtual void SetPosInfo(Protocol::PosInfo& Info);

	void SetMoveState(Protocol::MoveState State) { CurrentState = State; }
	Protocol::MoveState GetMoveState() { return PosInfo->state(); }
	Protocol::PosInfo* GetPosInfo() { return PosInfo; }

	/**
	* ProtoBuf
	*/
	void SetMovementInput(const FVector2d& Input) { MovementInput = Input; }
	void SetYaw(float yaw) { YawValue = yaw; }
	void SetControlPlayer() { ControlPlayer = true; }
	FVector2d& GetMovementInput() { return MovementInput; }
protected:
	bool ControlPlayer = false;
	FVector2D MovementInput;
	FVector2D LastDesiredInput;
	float YawValue = 0.f;

	const float MOVE_PACKET_SEND_DELAY = 0.2f;
	float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

	

	float Speed = 1024.f;
	bool bUpdated = false;
	bool bMoveKeyPressed = false;

protected:
	Protocol::MoveState CurrentState;
	Protocol::ObjectInfo* CharacterInfo;
	Protocol::PosInfo* PosInfo;
};