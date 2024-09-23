#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "R3/Camera/R3CameraMode.h"
#include "R3/Input/R3InputConfig.h"
#include "R3PawnData.generated.h"


UCLASS()
class UR3PawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UR3PawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	// PawnClass
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "R3|Pawn")
		TSubclassOf<APawn> PawnClass;

	/** Camera Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "R3|Camera")
		TSubclassOf<UR3CameraMode> DefaultCameraMode;

	/** Input configuration used by player controlled pawns to create input mappings and bind input actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "R3|InputConfig")
		TObjectPtr<UR3InputConfig> InputConfig;
};