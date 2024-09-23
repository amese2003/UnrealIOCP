// Copyright Epic Games, Inc. All Rights Reserved.

#include "R3.h"
#include "Modules/ModuleManager.h"
#include "R3/System/R3AssetManager.h"

/**
 * FLyraGameModule
 */
class FR3GameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FR3GameModule::StartupModule()
{
	UR3AssetManager::TestClone();
}

void FR3GameModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, R3, "R3" );

/*----------------
	SendBuffer
-----------------*/

