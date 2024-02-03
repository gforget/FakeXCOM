// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "UI/ActivateAbilityButton.h"

#include "AbilitySystemComponent.h"
#include "TBTacticalGameMode.h"

#include "ActorsObject/Unit.h"
#include "GameplayAbilities/UnitAbility.h"
#include "Managers/UnitAbilityManager.h"
#include "Utility/DebugHeader.h"

UActivateAbilityButton::UActivateAbilityButton()
{
	OnClicked.AddDynamic(this, &UActivateAbilityButton::OnClick);
}

void UActivateAbilityButton::OnClick()
{
	if (UnitRef)
	{
		if (UnitRef->CheckAbilityById(AbilityID))
		{
			UnitRef->TBTacticalGameMode->UnitAbilityManager->DeactivateAbilitySelectionMode();
			UnitRef->TryActivateAbilityByID(AbilityID, true);
		}
		else
		{
			DebugScreen(AbilityID +" cannot be activated for some reason", FColor::Red);
		}
	}
}
