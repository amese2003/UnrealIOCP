#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_ConnectServer.generated.h"

struct FComponentRequestHandle;
class APawn;


UCLASS()
class UGameFeatureAction_ConnectServer : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	/**
	* UGameFeatureAction Interface
	*/
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
};