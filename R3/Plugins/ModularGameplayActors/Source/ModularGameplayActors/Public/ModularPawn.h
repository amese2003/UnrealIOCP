#pragma once

#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "UObject/UObjectGlobals.h"

#include "ModularPawn.generated.h"

class UObject;

/** Minimal class that supports extension by game feature plugins */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularPawn : public APawn
{
	GENERATED_BODY()

public:
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

};
