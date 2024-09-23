#pragma once

#include "Camera/CameraComponent.h"
#include "R3CameraComponent.generated.h"

/** 전방 선언 */
class UR3CameraModeStack;
class UR3CameraMode;

/** 템플릿 전방선언 */
template <class TClass> class TSubclassOf;

/** (return type, delegate name) */
DECLARE_DELEGATE_RetVal(TSubclassOf<UR3CameraMode>, FR3CameraModeDelegate);

UCLASS()
class UR3CameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UR3CameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UR3CameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UR3CameraComponent>() : nullptr); }

	/**
	* CameraComponent interface
	*/
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	/**
	*  member Variable
	*/
	AActor* GetTargetActor() const { return GetOwner(); }
	void UpdateCameraModes();

	/** 카메라의 binding 기능을 지원하는 stack */
	UPROPERTY()
		TObjectPtr<UR3CameraModeStack> CameraModeStack;

	/** 현재 CameraMode를 가져오는 Delegate */
	FR3CameraModeDelegate DetermineCameraModeDelegate;
};