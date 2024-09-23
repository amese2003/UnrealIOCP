#pragma once


#include "ModularCharacter.h"
#include "Protocol.pb.h"
#include "R3Character.generated.h"


class UR3PawnExtensionComponent;
class UR3CameraComponent;

UCLASS()
class R3_API AR3Character : public AModularCharacter
{
	GENERATED_BODY()

public:
	AR3Character(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* ACharacter Interface
	*/
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) final;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "R3|Character")
		TObjectPtr<UR3PawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "R3|Character")
		TObjectPtr<UR3CameraComponent> CameraComponent;



public:
	void ResetCharacterInfo();
	void InitializePosition();
	void SetPlayerInfo(const Protocol::ObjectInfo& Info);
	Protocol::ObjectInfo* GetPlayerInfo() { return CharacterInfo; }
	void UpdatePosInfo();

	void SetMoveState(Protocol::MoveState State);
	Protocol::MoveState GetMoveState() { return CharacterInfo->mutable_pos_info()->state(); }


	

	


	Protocol::PosInfo* GetPosInfo() { return PosInfo; }
	void SetPosInfo(const Protocol::PosInfo& info);
	Protocol::MoveState GetDestMoveState() { return PosInfo->state(); }
	
	

private:
	class Protocol::ObjectInfo* CharacterInfo;
	class Protocol::PosInfo* PosInfo;
};