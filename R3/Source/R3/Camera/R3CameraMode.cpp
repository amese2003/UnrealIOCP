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
		// Weight가 1.0이면 other를 덮어쓰면 된다.
		*this = Other;
		return;
	}

	// Location + OtherWeight * (Other.Location - Location)
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	// Location과 같은 방식 Lerp (ControlRotation과 FieldOfView도 같음)
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
	// 우리가 앞서 UR3CameraMode를 생성하는 곳은 UR3CameraModeStack이였다.
	// - 해당 코드를 보면, GetOuter()를 FR3CameraMode로 R3CameraComponent로 설정하였다.
	// - UR3CameraModeStack::GetCameraModeInstance() 확인
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
		// BaseEyeHeight를 고려하여, ViewLocation을 반환함
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
		// GetViewRotation() 확인, 보통 Pawn이 ControlRotation을 반환
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UR3CameraMode::UpdateBlending(float DeltaTime)
{
	// BlendAlpha를 DeltaTime을 통해 계산
	if (BlendTime > 0.f)
	{
		// BlendTime은 Blending 과정 중 시간(초)
		// - BlendAlpha는 0 -> x로 변화하는 과정:(dx)
		// - DeltaTime을 활용하여, BlendTime을 1로 볼 경우, 진행 정도를 누적
		BlendAlpha += (DeltaTime / BlendTime);

	}
	else
	{
		BlendAlpha = 1.f;
	}

	// BlendWeight를 [0, 1]를 BlendFunction에 맞게 재매핑
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
	// Actor를 활용하여, Pivot[Location[Rotation]]을 계산하여, View 업데이트
	UpdateView(DeltaTime);

	// BlendWeight를 DeltaTime을 활용하여, BlendAlpha 계산 후, BlendFunction에 맞게 재-매핑하여 최종 계산
	UpdateBlending(DeltaTime);
}

void UR3CameraMode::UpdateView(float DeltaTime)
{
	// CameraMode를 가지고 있는 CameraComponent의 Owner인 Character(Pawn)을 활용하여, PivotLocation/Rotation을 반환함
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	// Pitch 값에 대해 Min/Max를 Clamp시킴
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// FLyraCloneCameraModeView에 PivotLocation/Rotation 설정
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	// PivotRotation을 똑같이 ControlRotation으로 활용
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// 정리하면, Character의 Location과 ControlRotation을 활용하여, View를 업데이트함
}



UR3CameraModeStack::UR3CameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UR3CameraMode* UR3CameraModeStack::GetCameraModeInstance(TSubclassOf<UR3CameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstance에서 먼저 생성이 되어있는지 확인 후, 반환한다.
	for (UR3CameraMode* CameraMode : CameraModeInstances)
	{
		// CameraMode는 UClass를 비교한다
		// - 즉, CameraMode는 클래스 타입에 하나만 생기게된다.
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass에 걸맞는 CameraMode의 인스턴스가 없으면 생성한다.
	UR3CameraMode* NewCameraMode = NewObject<UR3CameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// 여기서 우리는 CameraModeInstance는 CameraModeClass에 맞는 인스턴스를 가지고 있는 컨테이너라는 것을 알 수 있다.
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
		// CameraModeStack[0] 가장 최근에 이미 CameraMode가 Stacking 되었으니까, 리턴
		return;
	}

	// ExistingStackIndex는 CameraModeStack에서 CameraMode에 맞는 Index를 찾음
	// ExistingStackContribution은 위에서 아래로 최종 BlendWeight 값을 찾기 위해 초기값으로 1.0으로 설정
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
			// BlendWeight를 CameraMode에 추가해주자.
			// - 여기서 ExistingStackContribution 우리가 찾는 CameraMode를 찾았으니까, 누적된 BlendWeight와 함께 BlendWeight를 곱하여 루프를 빠져나온다
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}
	// 해당 루프의 동작 원리는 앞서 적어놓은 표를 확인하며 이해하자.

	// 우리는 CameraMode를 Top으로 반영하기 위해, 당연히 중간에 있었다면, 제거하여 다시 Push 해줘야한다.
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		// 없다. 당연히 Contribution은 0으로 세팅해줘야한다.
		ExistingStackContribution = 0.f;
	}

	// BlendTime이 0보다 크다는 것은 Blend를 얼마 시간동안 진행함을 의미. 따라서, ExistingStackContribution을 적용
	// - 따라서 Blend 하지 않는다면, BlendWeight를 1.0을 넣어 새로 넣는 CameraMode만 적용할 것이다.
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	// 근데 Array를 Stack처러 사용하는 것은 알겠는데, 끝부분도 아니고 0에?
	// - push, Pop 메서드와 같이 그냥 last에 넣는게 좋을텐데?
	CameraModeStack.Insert(CameraMode, 0);

	// 앞서 봤듯이, 마지막은 항상 1.0이 되어야 함!
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UR3CameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();

	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack을 순회하며, CameraMode를 업데이트한다
	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; StackIndex++)
	{
		UR3CameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// UpdateCameraMode를 확인해보자..
		CameraMode->UpdateCameraMode(DeltaTime);

		// 만약 하나라도 CameraMode가 BlendWeight가 1.0에 도달했다면, 그 이후 CameraMode를 제거한다
		if (CameraMode->BlendWeight >= 1.f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		// 생각해보면 이거 때문에 Pop, Push를 안했을지도...
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

	// CameraModeStack은 Bottom -> Top 순서로 Blend 진행한다.
	const UR3CameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	// 가장 아래 (Bottom)은 BlendWeight가 1.0이다...
	OutCameraModeView = CameraMode->View;

	// 이미 Index = [StackSize - 1] 이미 OutCameraModeView로 지정했으므로, StackSize - 2부터 순회하면 된다.
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; StackIndex--)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);


		// R3CameraModeView Blend 함수를 구현하자...
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);

	}
}

void UR3CameraModeStack::EvaluateStack(float DeltaTime, FR3CameraModeView& OutCameraModeView)
{
	// Top -> Bottom [ 0 -> Num ] 까지 순차적으로 Stack에 있는 CameraMode 업데이트
	UpdateStack(DeltaTime);

	// Bottom -> Top 까지 CameraModeStack에 대해 Blending
	BlendStack(OutCameraModeView);
}
