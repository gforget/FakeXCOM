// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAbilityManager.h"

void UUnitAbilityManager::ActivateAbilitySelectionMode(
	UUnitAbility* CurrentSelectedAbilityArg,
	FGameplayAbilitySpecHandle CurrentSelectedAbilityHandleArg
	)
{
	CurrentSelectedAbility = CurrentSelectedAbilityArg;
	CurrentSelectedAbilityHandle = CurrentSelectedAbilityHandleArg;
	
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
