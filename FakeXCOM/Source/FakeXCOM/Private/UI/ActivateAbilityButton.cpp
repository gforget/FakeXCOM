// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "UI/ActivateAbilityButton.h"

#include "AbilitySystemComponent.h"
#include "TBTacticalGameMode.h"

#include "ActorsObject/Unit.h"
#include "GameplayAbilities/UnitAbility.h"
#include "Managers/UnitAbilityManager.h"

UActivateAbilityButton::UActivateAbilityButton()
{
	OnClicked.AddDynamic(this, &UActivateAbilityButton::OnClick);
}

void UActivateAbilityButton::OnClick()
{
	if (UnitRef && !UnitRef->TBTacticalGameMode->UnitAbilityManager->CurrentSelectedAbility->GetIsDisabled(UnitRef))
	{
		UnitRef->TBTacticalGameMode->UnitAbilityManager->DeactivateAbilitySelectionMode();
		UnitRef->AbilitySystemComponent->TryActivateAbility(AbilityHandle);
	}
}
