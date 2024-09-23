#include "R3CameraMode_ThirdPerson.h"
#include "Curves/CurveVector.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3CameraMode_ThirdPerson)

UR3CameraMode_ThirdPerson::UR3CameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR3CameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// TargetOffsetCurve가 오버라이드 되어있다면, Curve에 값을 가져와서 적용
	// - Camera 관점에서 Character의 어느 부분에 Target으로 할 지 결정하는 것으로 이해

	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}

}
