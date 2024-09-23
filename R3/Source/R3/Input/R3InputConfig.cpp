#include "R3InputConfig.h"
#include "R3/R3LogChannel.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3InputConfig)

UR3InputConfig::UR3InputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UR3InputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// NativeInputActions를 순회하며, Input으로 들어온 InputTag가 있는지 확인한다
	// - 있으면, 그에 따른 InputAction을 반환하지만, 없다면 그냥 nullptr을 반환한다..
	for (const FR3InputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogR3, Error, TEXT("Can't Find NativeInputAction For InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UR3InputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return nullptr;
}
