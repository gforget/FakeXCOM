// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "Managers/UnitAbilityManager.h"

void UUnitAbilityManager::ActivateAbilitySelectionMode(FString CurrentSelectedAbilityIDArg)
{
	CurrentSelectedAbilityID = CurrentSelectedAbilityIDArg;
	
	bAbilitySelectionMode = true;
	OnAbilitySelectionModeChangeEvent.Broadcast(bAbilitySelectionMode);
}

void UUnitAbilityManager::DeactivateAbilitySelectionMode()
{
	bAbilitySelectionMode = false;
	OnAbilitySelectionModeChangeEvent.Broadcast(bAbilitySelectionMode);
}

bool UUnitAbilityManager::GetAbilitySelectionMode()
{
	return bAbilitySelectionMode;
}
