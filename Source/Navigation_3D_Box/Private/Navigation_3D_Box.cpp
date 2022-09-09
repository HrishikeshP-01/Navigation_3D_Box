// Copyright Epic Games, Inc. All Rights Reserved.

#include "Navigation_3D_Box.h"

#define LOCTEXT_NAMESPACE "FNavigation_3D_BoxModule"

void FNavigation_3D_BoxModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FNavigation_3D_BoxModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNavigation_3D_BoxModule, Navigation_3D_Box)