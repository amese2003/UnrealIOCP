#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"

#include "R3NetworkExecutionComponent.generated.h"

class UR3ExperienceDefinition;
class AR3SocketPlayerController;

UCLASS(Blueprintable, Abstract)
class UR3NetworkExecutionComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UR3NetworkExecutionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	*	UActorComponent's Interface
	*/
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	TSharedPtr<NetworkManager> NetworkManagerInstance;
};