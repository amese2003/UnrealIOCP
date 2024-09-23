#pragma once
#include "PlayerMappableInputConfig.h"
#include "R3MappableConfigPair.generated.h"


//class UPlayerMappableInputConfig;

USTRUCT()
struct FR3MappableConfigPair
{
	GENERATED_BODY()

public:
	FR3MappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
		bool bShouldActivateAutomatically = false;
};