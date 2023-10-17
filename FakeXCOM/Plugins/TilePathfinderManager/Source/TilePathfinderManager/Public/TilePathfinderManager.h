// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTilePathfinderManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	void AddMenu(FMenuBarBuilder& MenuBuilder);
	void FillMenu(FMenuBuilder& MenuBuilder);
	void OnPathfinderMenuButtonClicked();

#if WITH_EDITOR
	UFUNCTION()
	void OnActorSelected(const TArray<UObject*>& objectsSelected, bool value);
#endif
};
