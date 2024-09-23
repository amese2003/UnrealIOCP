#pragma once

#include "Camera/PlayerCameraManager.h"
#include "R3PlayerCameraManager.generated.h"

/**
* Controller에 바인딩한 CameraManager
*/

#define R3_CAMERA_DEFAULT_FOV (80.0f)
#define R3_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define R3_CAMERA_DEFAULT_PITCH_MAX (89.0f)


UCLASS()
class AR3PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AR3PlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};