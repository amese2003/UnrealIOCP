#pragma once

#include "R3CameraMode.generated.h"

/** ���漱�� */
class UR3CameraComponent;


/**
* [0, 1]�� BlendFunction�� �°� ������� ���� Ÿ��
*/
UENUM(BlueprintType)
enum class ER3CameraModeBlendFunction : uint8
{
	Linear,
	/**
	* EaseIn/Out�� exponent ���� ���� �����ȴ�..
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

/** Camera Blending ��� ���� */
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

	/** CameraMode�� ���� ������ CameraModeView */
	FR3CameraModeView View;

	/** Camera Mode�� FOV */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
		float FieldOfView;

	/** View�� ���� Pitch [Min, Max] */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
		float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
		float ViewPitchMax;


	/** �󸶵��� Blend�� �������� �ð� �ǹ� */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
		float BlendTime;

	/** ���� Blend �� [0, 1] */
	float BlendAlpha;

	/**
	*  �ش� CameraMode�� ���� Blend ��
	*  �ռ� BlendAlpha�� ���� ���� �����Ͽ� ���� BlendWeight�� ��� (�ڵ带 ���� ��������..)
	*/
	float BlendWeight;

	/**
	* EaseIn / Out�� ����� Exponent
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
		float BlendExponent;

	/** Blend function */
	ER3CameraModeBlendFunction BlendFunction;

};

/** Camera Binding�� ����ϴ� ��ü */
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

	// ������ ī�޶� ��带 ����
	UPROPERTY()
		TArray<TObjectPtr<UR3CameraMode>> CameraModeInstances;

	// ī�޶� Matrix Blend ������Ʈ ���� ��
	UPROPERTY()
		TArray<TObjectPtr<UR3CameraMode>> CameraModeStack;
};