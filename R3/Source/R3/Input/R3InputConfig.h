#pragma once

#include "Containers/Array.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "R3InputConfig.generated.h"

// Foward Declation
class UInputAction;

/** R3InputAction�� GameplayTag�� InputAction�� �����ϴ� ���� Ŭ���� */
USTRUCT(BlueprintType)
struct FR3InputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
		FGameplayTag InputTag;
};

/**
*  R3InputConfig�� Ȱ��ȭ ������ InputAction�� ������ �ִ� Ŭ�����̴�.
*  - �ڵ��� ����...
*/
UCLASS(BlueprintType)
class UR3InputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UR3InputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	/**
	*  member Variables
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
		TArray<FR3InputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
		TArray<FR3InputAction> AbilityInputActions;

};