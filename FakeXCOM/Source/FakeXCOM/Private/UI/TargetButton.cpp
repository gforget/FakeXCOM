// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "UI/TargetButton.h"

#include "TBTacticalGameMode.h"

#include "Managers/TargetManager.h"
#include "Managers/UnitAbilityManager.h"

UTargetButton::UTargetButton()
{
	OnClicked.AddDynamic(this, &UTargetButton::OnClick);
}

UTargetButton::~UTargetButton()
{
	UnbindEvents();
}

void UTargetButton::OnClick()
{
	if (const ATBTacticalGameMode* TBTacticalGameModePtr = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>())
	{
		if (SelectDefaultAbility)
		{
			AUnit* SelectedUnit = TBTacticalGameModePtr->UnitManager->GetCurrentlySelectedUnit();
			UUnitAbility* DefaultAbility = SelectedUnit->OwnedAbilities[SelectedUnit->DefaultAbilitiesIndexes[0]];
			const FGameplayAbilitySpecHandle DefaultAbilityHandle = SelectedUnit->OwnedAbilitiesHandle[SelectedUnit->DefaultAbilitiesIndexes[0]];
			
			TBTacticalGameModePtr->UnitAbilityManager->ActivateAbilitySelectionMode(DefaultAbility, DefaultAbilityHandle);
		}
		
		TBTacticalGameModePtr->TargetManager->SelectTarget(TargetActorIndex);
	} 
}

void UTargetButton::UnbindEvents()
{
}


