#include "R3PlayerCameraManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3PlayerCameraManager)

AR3PlayerCameraManager::AR3PlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultFOV = R3_CAMERA_DEFAULT_FOV;
	ViewPitchMin = R3_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = R3_CAMERA_DEFAULT_PITCH_MAX;
}
