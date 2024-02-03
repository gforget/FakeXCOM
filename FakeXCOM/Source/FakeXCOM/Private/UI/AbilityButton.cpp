// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "UI/AbilityButton.h"

#include "UI/LevelUI.h"
#include "TBTacticalGameMode.h"

#include "Managers/UnitAbilityManager.h"

UAbilityButton::UAbilityButton()
{
	OnClicked.AddDynamic(this, &UAbilityButton::OnClick);
}

UAbilityButton::~UAbilityButton()
{
	UnbindEvents();
}

void UAbilityButton::OnClick()
{
	if (const ATBTacticalGameMode* TBTacticalGameModePtr = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>())
	{
		//TODO: The Ref Variables are set in blueprint, might want to change that
		TBTacticalGameModePtr->UnitAbilityManager->ActivateAbilitySelectionMode(AbilityID);
	}
}

void UAbilityButton::UnbindEvents()
{
	OnClicked.RemoveDynamic(this, &UAbilityButton::OnClick);
}
