#pragma once

#include "R3CameraMode.h"
#include "R3CameraMode_ThirdPerson.generated.h"

class UCurveVector;

UCLASS(Abstract, Blueprintable)
class UR3CameraMode_ThirdPerson : public UR3CameraMode
{
	GENERATED_BODY()

public:
	UR3CameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	/**
	*  ULyraCloneCameraMode interface
	*/
	virtual void UpdateView(float DeltaTime) override;


	/**
	*	Member variables
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
		TObjectPtr<const UCurveVector> TargetOffsetCurve;
};