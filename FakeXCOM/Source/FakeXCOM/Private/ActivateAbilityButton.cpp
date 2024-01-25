// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "ActivateAbilityButton.h"

#include "AbilitySystemComponent.h"
#include "TBTacticalGameMode.h"
#include "Unit.h"
#include "UnitAbility.h"
#include "UnitAbilityManager.h"

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
