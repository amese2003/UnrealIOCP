#include "R3CameraMode.h"
#include "R3PlayerCameraManager.h"
#include "R3CameraComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3CameraMode)

FR3CameraModeView::FR3CameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(R3_CAMERA_DEFAULT_FOV)
{
}

void FR3CameraModeView::Blend(const FR3CameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if (OtherWeight >= 1.0f)
	{
		// Weight�� 1.0�̸� other�� ����� �ȴ�.
		*this = Other;
		return;
	}

	// Location + OtherWeight * (Other.Location - Location)
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	// Location�� ���� ��� Lerp (ControlRotation�� FieldOfView�� ����)
	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

UR3CameraMode::UR3CameraMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FieldOfView = R3_CAMERA_DEFAULT_FOV;
	ViewPitchMin = R3_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = R3_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

	BlendFunction = ER3CameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

UR3CameraComponent* UR3CameraMode::GetR3CameraComponent() const
{	
	// �츮�� �ռ� UR3CameraMode�� �����ϴ� ���� UR3CameraModeStack�̿���.
	// - �ش� �ڵ带 ����, GetOuter()�� FR3CameraMode�� R3CameraComponent�� �����Ͽ���.
	// - UR3CameraModeStack::GetCameraModeInstance() Ȯ��
	return CastChecked<UR3CameraComponent>(GetOuter());
}

AActor* UR3CameraMode::GetTargetActor() const
{
	const UR3CameraComponent* CameraComponent = GetR3CameraComponent();
	return CameraComponent->GetTargetActor();
}

FVector UR3CameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// BaseEyeHeight�� ����Ͽ�, ViewLocation�� ��ȯ��
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UR3CameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// GetViewRotation() Ȯ��, ���� Pawn�� ControlRotation�� ��ȯ
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UR3CameraMode::UpdateBlending(float DeltaTime)
{
	// BlendAlpha�� DeltaTime�� ���� ���
	if (BlendTime > 0.f)
	{
		// BlendTime�� Blending ���� �� �ð�(��)
		// - BlendAlpha�� 0 -> x�� ��ȭ�ϴ� ����:(dx)
		// - DeltaTime�� Ȱ���Ͽ�, BlendTime�� 1�� �� ���, ���� ������ ����
		BlendAlpha += (DeltaTime / BlendTime);

	}
	else
	{
		BlendAlpha = 1.f;
	}

	// BlendWeight�� [0, 1]�� BlendFunction�� �°� �����
	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case ER3CameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case ER3CameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ER3CameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ER3CameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	default:
		checkf(false, TEXT("UpdateBlending : Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UR3CameraMode::UpdateCameraMode(float DeltaTime)
{
	// Actor�� Ȱ���Ͽ�, Pivot[Location[Rotation]]�� ����Ͽ�, View ������Ʈ
	UpdateView(DeltaTime);

	// BlendWeight�� DeltaTime�� Ȱ���Ͽ�, BlendAlpha ��� ��, BlendFunction�� �°� ��-�����Ͽ� ���� ���
	UpdateBlending(DeltaTime);
}

void UR3CameraMode::UpdateView(float DeltaTime)
{
	// CameraMode�� ������ �ִ� CameraComponent�� Owner�� Character(Pawn)�� Ȱ���Ͽ�, PivotLocation/Rotation�� ��ȯ��
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	// Pitch ���� ���� Min/Max�� Clamp��Ŵ
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// FLyraCloneCameraModeView�� PivotLocation/Rotation ����
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	// PivotRotation�� �Ȱ��� ControlRotation���� Ȱ��
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// �����ϸ�, Character�� Location�� ControlRotation�� Ȱ���Ͽ�, View�� ������Ʈ��
}



UR3CameraModeStack::UR3CameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UR3CameraMode* UR3CameraModeStack::GetCameraModeInstance(TSubclassOf<UR3CameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstance���� ���� ������ �Ǿ��ִ��� Ȯ�� ��, ��ȯ�Ѵ�.
	for (UR3CameraMode* CameraMode : CameraModeInstances)
	{
		// CameraMode�� UClass�� ���Ѵ�
		// - ��, CameraMode�� Ŭ���� Ÿ�Կ� �ϳ��� ����Եȴ�.
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass�� �ɸ´� CameraMode�� �ν��Ͻ��� ������ �����Ѵ�.
	UR3CameraMode* NewCameraMode = NewObject<UR3CameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// ���⼭ �츮�� CameraModeInstance�� CameraModeClass�� �´� �ν��Ͻ��� ������ �ִ� �����̳ʶ�� ���� �� �� �ִ�.
	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UR3CameraModeStack::PushCameraMode(TSubclassOf<UR3CameraMode>& CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	UR3CameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// CameraModeStack[0] ���� �ֱٿ� �̹� CameraMode�� Stacking �Ǿ����ϱ�, ����
		return;
	}

	// ExistingStackIndex�� CameraModeStack���� CameraMode�� �´� Index�� ã��
	// ExistingStackContribution�� ������ �Ʒ��� ���� BlendWeight ���� ã�� ���� �ʱⰪ���� 1.0���� ����
	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.f;


	/**
	*	BlendWeight				|		ExistingStackContribution		|		ExistingStackContribution (accumlated)
	*	0.1f					|		(1.0f) * 0.1f = 0.1f			|		(1.0f - 0.1f) = 0.9f;
	*	0.3f					|		(0.9f) * 0.3f = 0.27f			|		(1.0f - 0.3f) * 0.9f = 0.63f
	*	0.6f					|		(0.63f) * 0.6f = 0.378f			|		(1.0f - 0.6f) * 0.63f = 0.252f
	*	1.0f					|		(0.252f) * 1.0f = 0.252f		|
	*							|		0.1f + 0.27f + 0.378f + 0.252f = 1.0f;
	*
	*/

	for (int32 StackIndex = 0; StackIndex < StackSize; StackIndex++)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			// BlendWeight�� CameraMode�� �߰�������.
			// - ���⼭ ExistingStackContribution �츮�� ã�� CameraMode�� ã�����ϱ�, ������ BlendWeight�� �Բ� BlendWeight�� ���Ͽ� ������ �������´�
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}
	// �ش� ������ ���� ������ �ռ� ������� ǥ�� Ȯ���ϸ� ��������.

	// �츮�� CameraMode�� Top���� �ݿ��ϱ� ����, �翬�� �߰��� �־��ٸ�, �����Ͽ� �ٽ� Push ������Ѵ�.
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		// ����. �翬�� Contribution�� 0���� ����������Ѵ�.
		ExistingStackContribution = 0.f;
	}

	// BlendTime�� 0���� ũ�ٴ� ���� Blend�� �� �ð����� �������� �ǹ�. ����, ExistingStackContribution�� ����
	// - ���� Blend ���� �ʴ´ٸ�, BlendWeight�� 1.0�� �־� ���� �ִ� CameraMode�� ������ ���̴�.
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	// �ٵ� Array�� Stackó�� ����ϴ� ���� �˰ڴµ�, ���κе� �ƴϰ� 0��?
	// - push, Pop �޼���� ���� �׳� last�� �ִ°� �����ٵ�?
	CameraModeStack.Insert(CameraMode, 0);

	// �ռ� �õ���, �������� �׻� 1.0�� �Ǿ�� ��!
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UR3CameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();

	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack�� ��ȸ�ϸ�, CameraMode�� ������Ʈ�Ѵ�
	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; StackIndex++)
	{
		UR3CameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// UpdateCameraMode�� Ȯ���غ���..
		CameraMode->UpdateCameraMode(DeltaTime);

		// ���� �ϳ��� CameraMode�� BlendWeight�� 1.0�� �����ߴٸ�, �� ���� CameraMode�� �����Ѵ�
		if (CameraMode->BlendWeight >= 1.f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		// �����غ��� �̰� ������ Pop, Push�� ����������...
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UR3CameraModeStack::BlendStack(FR3CameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack�� Bottom -> Top ������ Blend �����Ѵ�.
	const UR3CameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	// ���� �Ʒ� (Bottom)�� BlendWeight�� 1.0�̴�...
	OutCameraModeView = CameraMode->View;

	// �̹� Index = [StackSize - 1] �̹� OutCameraModeView�� ���������Ƿ�, StackSize - 2���� ��ȸ�ϸ� �ȴ�.
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; StackIndex--)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);


		// R3CameraModeView Blend �Լ��� ��������...
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);

	}
}

void UR3CameraModeStack::EvaluateStack(float DeltaTime, FR3CameraModeView& OutCameraModeView)
{
	// Top -> Bottom [ 0 -> Num ] ���� ���������� Stack�� �ִ� CameraMode ������Ʈ
	UpdateStack(DeltaTime);

	// Bottom -> Top ���� CameraModeStack�� ���� Blending
	BlendStack(OutCameraModeView);
}
