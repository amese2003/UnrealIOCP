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
};