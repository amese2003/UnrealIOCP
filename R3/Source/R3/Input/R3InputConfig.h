#pragma once

#include "Containers/Array.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "R3InputConfig.generated.h"

// Foward Declation
class UInputAction;

/** R3InputAction은 GameplayTag와 InputAction을 연결하는 레퍼 클래스 */
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
*  R3InputConfig는 활성화 가능한 InputAction을 가지고 있는 클래스이다.
*  - 코드좀 보자...
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