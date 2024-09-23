#pragma once

#include "R3CameraMode.generated.h"

/** 전방선언 */
class UR3CameraComponent;


/**
* [0, 1]를 BlendFunction에 맞게 재매핑을 위한 타입
*/
UENUM(BlueprintType)
enum class ER3CameraModeBlendFunction : uint8
{
	Linear,
	/**
	* EaseIn/Out은 exponent 값에 의해 조절된다..
	*/
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};

/**
*	FLyraCloneCameraModeView
*/
struct FR3CameraModeView
{
	FR3CameraModeView();

	void Blend(const FR3CameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/** Camera Blending 대상 유닛 */
UCLASS(Abstract, NotBlueprintable)
class UR3CameraMode : public UObject
{
	GENERATED_BODY()

public:
	UR3CameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UR3CameraComponent* GetR3CameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;
	void UpdateBlending(float DeltaTime);
	void UpdateCameraMode(float DeltaTime);

	virtual void UpdateView(float DeltaTime);

	/** CameraMode에 의해 생성된 CameraModeView */
	FR3CameraModeView View;

	/** Camera Mode의 FOV */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
		float FieldOfView;

	/** View에 대한 Pitch [Min, Max] */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
		float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
		float ViewPitchMax;


	/** 얼마동안 Blend를 진행할지 시간 의미 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
		float BlendTime;

	/** 선형 Blend 값 [0, 1] */
	float BlendAlpha;

	/**
	*  해당 CameraMode의 최종 Blend 값
	*  앞서 BlendAlpha의 선형 값을 매핑하여 최종 BlendWeight를 계산 (코드를 보며 이해하자..)
	*/
	float BlendWeight;

	/**
	* EaseIn / Out에 사용한 Exponent
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
		float BlendExponent;

	/** Blend function */
	ER3CameraModeBlendFunction BlendFunction;

};

/** Camera Binding을 담당하는 객체 */
UCLASS()
class UR3CameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UR3CameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	/**
	*	Member Methods
	*/
	UR3CameraMode* GetCameraModeInstance(TSubclassOf<UR3CameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<UR3CameraMode>& CameraModeClass);
	void UpdateStack(float DeltaTime);
	void BlendStack(FR3CameraModeView& OutCameraModeView) const;
	void EvaluateStack(float DeltaTime, FR3CameraModeView& OutCameraModeView);


	/**
	*	Member Variable
	*/

	// 생성한 카메라 모드를 관리
	UPROPERTY()
		TArray<TObjectPtr<UR3CameraMode>> CameraModeInstances;

	// 카메라 Matrix Blend 업데이트 진행 중
	UPROPERTY()
		TArray<TObjectPtr<UR3CameraMode>> CameraModeStack;
};