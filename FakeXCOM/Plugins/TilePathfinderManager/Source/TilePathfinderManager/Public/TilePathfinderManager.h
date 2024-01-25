// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#if WITH_EDITOR

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
	
	UFUNCTION()
	void OnActorSelected(const TArray<UObject*>& objectsSelected, bool value);

};

#endif
