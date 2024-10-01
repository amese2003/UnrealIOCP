#include "R3Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "R3PawnExtensionComponent.h"
#include "R3/Camera/R3CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3Character)

AR3Character::AR3Character(const FObjectInitializer& ObjectInitializer)
{
	// 기본적으로 Tick을 꺼버리자
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.

	// PawnExtComponent 생성
	PawnExtComponent = CreateDefaultSubobject<UR3PawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// CameraComponent 생성
	{
		CameraComponent = CreateDefaultSubobject<UR3CameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 75.f));
	}

	
}

void AR3Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Pawn이 Possess로서, Controller와 PlayerState 정보 접근이 가능한 상태가 되었음.
	// - SetupPlayerInputComponent 확인

	PawnExtComponent->SetupPlayerInputComponent();
}
