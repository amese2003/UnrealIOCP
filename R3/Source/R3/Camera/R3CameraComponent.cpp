#include "R3CameraComponent.h"
#include "R3CameraMode.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3CameraComponent)

UR3CameraComponent::UR3CameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CameraModeStack(nullptr)
{
}

void UR3CameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		// 초기화 (BeginPlay와 같은)가 딱히 필요없는 객체로 NewObject로 생성
		CameraModeStack = NewObject<UR3CameraModeStack>(this);
	}
}

void UR3CameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);

	// 해당 함수는 HeroComponent에서 PawnData에서 DefaultCameraMode를 가져와서 CameraModeStack에 추가하며, CameraMode 블랜딩 준비 
	UpdateCameraModes();

	// EvaluateStack은 CameraModeStack에 있는 CameraMode를 업데이트(+블랜딩)하고 CameraModeStack을 Bottom-Top까지 업데이트 된 CameraMode들을 Lerp를 진행해준다.
	// - 이에 대한 결과는 CameraModeView에 캐싱된다
	FR3CameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			// PlayerController의 ControlRotation을 계산된 CameraModeView의 ControlRotation으로 업데이트
			//	- SetControlRotation을 외우기 보다, 한번 코드를 보자
			//		- 해당 함수는 PC가 Possess하고 있는 Pawn의 RootComponent에 ControlRotation을 반영한다. (== 조정하고 있는 캐릭터에 회전을?)
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// Camera의 Location과 Rotation을 반영하자.
	//	- 시간되면 SetWorldLocationAndRotation을 보면서 -> UpdateChildTransform을 봐야한다
	//	- SceneGraph 관계 업데이트를 이해 할 수 있다.. 
	// 간단하게 정리하면, CameraComponent에 대해 Parent의 SceneGraph 관계를 고려하여 업데이트 진행
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	// FOV 업데이트
	FieldOfView = CameraModeView.FieldOfView;

	/**
	*  여기서 우리는 ControlRotation vs Rotation의 차이점을 이해 할 수있다...
	*	- ControlRotation은 PC가 조정하는 Pawn의 Rotation을 적용한 값이다
	*	- 그에 반해, Rotation은 Camera에 반영하는 Rotation이란 뜻이다.
	*/

	// FMinimalViewInfo를 업데이트 해준다.
	// - CameraComponent의 변화 시점을 반영해서 최종 렌더링까지 반영하게 됨
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;

	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}

}

void UR3CameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate는 CameraMode Class를 반환한다
	// - 해당 delegate는 HeroComponent의 멤버 변수로 바인딩되어있다.
	if (DetermineCameraModeDelegate.IsBound())
	{
		if (TSubclassOf<UR3CameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			// 
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}
