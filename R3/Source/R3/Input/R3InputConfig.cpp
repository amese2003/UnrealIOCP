#include "R3InputConfig.h"
#include "R3/R3LogChannel.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3InputConfig)

UR3InputConfig::UR3InputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UR3InputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// NativeInputActions�� ��ȸ�ϸ�, Input���� ���� InputTag�� �ִ��� Ȯ���Ѵ�
	// - ������, �׿� ���� InputAction�� ��ȯ������, ���ٸ� �׳� nullptr�� ��ȯ�Ѵ�..
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
