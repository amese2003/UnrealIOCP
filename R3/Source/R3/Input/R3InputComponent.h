#pragma once

#include "EnhancedInputComponent.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "R3InputConfig.h"
#include "R3InputComponent.generated.h"

UCLASS()
class UR3InputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UR3InputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	*  member methods
	*/
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UR3InputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UR3InputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);


};

template<class UserClass, typename FuncType>
inline void UR3InputComponent::BindNativeAction(const UR3InputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	// ���⼭ �� �� �ֵ���, InputConfig�� Ȱ�� ������ InputAction�� ��� �ִ�
	// - ���� InputConfig�� ���� InputAction�� Binding��Ű��, nullptr�� ��ȯ�ϸ�, ���ε��ϴµ� �����Ѵ�!
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
inline void UR3InputComponent::BindAbilityActions(const UR3InputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	// AbilityAction�� ���ؼ��� �׳� ��� InputAction�� �� ���ε� ��Ų��!
	for (const FR3InputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
