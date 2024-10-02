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

	virtual void PlayerTick(float DeltaTime) override;

	/**
	* ProtoBuf
	*/
	bool IsNetworking();

	virtual void UpdateSocketMovement(float DeltaTime);

	virtual void UpdateIdle();
	virtual void UpdateMoving(float DeltaTime);
	virtual void UpdateSkill();
	virtual void UpdateDead();

	void ResetCharacterInfo();
	void InitializePosition();
	void SetPlayerInfo(const Protocol::ObjectInfo& Info);
	void SetPosInfo(Protocol::PosInfo& Info);

	Protocol::ObjectInfo* GetPlayerInfo() { return CharacterInfo; }
	virtual void MoveToNextPos(float deltaTime) {};
	virtual void CheckUpdateFlag(float deltaTime, bool ForceUpdate = false) {};

	

	void SetMoveState(Protocol::MoveState State) { CurrentState = State; bUpdated = true; }
	Protocol::MoveState GetMoveState() { return PosInfo->state(); }
	Protocol::PosInfo* GetPosInfo() { return PosInfo; }

	void SyncPosition(float DeltaTime);



protected:
	

	float Speed = 1024.f;
	bool bUpdated = false;

protected:
	Protocol::MoveState CurrentState;
	Protocol::ObjectInfo* CharacterInfo;
	Protocol::PosInfo* PosInfo;
};