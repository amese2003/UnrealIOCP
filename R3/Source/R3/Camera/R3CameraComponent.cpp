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
		// �ʱ�ȭ (BeginPlay�� ����)�� ���� �ʿ���� ��ü�� NewObject�� ����
		CameraModeStack = NewObject<UR3CameraModeStack>(this);
	}
}

void UR3CameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);

	// �ش� �Լ��� HeroComponent���� PawnData���� DefaultCameraMode�� �����ͼ� CameraModeStack�� �߰��ϸ�, CameraMode ���� �غ� 
	UpdateCameraModes();

	// EvaluateStack�� CameraModeStack�� �ִ� CameraMode�� ������Ʈ(+����)�ϰ� CameraModeStack�� Bottom-Top���� ������Ʈ �� CameraMode���� Lerp�� �������ش�.
	// - �̿� ���� ����� CameraModeView�� ĳ�̵ȴ�
	FR3CameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			// PlayerController�� ControlRotation�� ���� CameraModeView�� ControlRotation���� ������Ʈ
			//	- SetControlRotation�� �ܿ�� ����, �ѹ� �ڵ带 ����
			//		- �ش� �Լ��� PC�� Possess�ϰ� �ִ� Pawn�� RootComponent�� ControlRotation�� �ݿ��Ѵ�. (== �����ϰ� �ִ� ĳ���Ϳ� ȸ����?)
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// Camera�� Location�� Rotation�� �ݿ�����.
	//	- �ð��Ǹ� SetWorldLocationAndRotation�� ���鼭 -> UpdateChildTransform�� �����Ѵ�
	//	- SceneGraph ���� ������Ʈ�� ���� �� �� �ִ�.. 
	// �����ϰ� �����ϸ�, CameraComponent�� ���� Parent�� SceneGraph ���踦 ����Ͽ� ������Ʈ ����
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	// FOV ������Ʈ
	FieldOfView = CameraModeView.FieldOfView;

	/**
	*  ���⼭ �츮�� ControlRotation vs Rotation�� �������� ���� �� ���ִ�...
	*	- ControlRotation�� PC�� �����ϴ� Pawn�� Rotation�� ������ ���̴�
	*	- �׿� ����, Rotation�� Camera�� �ݿ��ϴ� Rotation�̶� ���̴�.
	*/

	// FMinimalViewInfo�� ������Ʈ ���ش�.
	// - CameraComponent�� ��ȭ ������ �ݿ��ؼ� ���� ���������� �ݿ��ϰ� ��
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

	// DetermineCameraModeDelegate�� CameraMode Class�� ��ȯ�Ѵ�
	// - �ش� delegate�� HeroComponent�� ��� ������ ���ε��Ǿ��ִ�.
	if (DetermineCameraModeDelegate.IsBound())
	{
		if (TSubclassOf<UR3CameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			// 
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}
