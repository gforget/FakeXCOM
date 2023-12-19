// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetButton.h"

#include "TargetManager.h"
#include "TBTacticalGameMode.h"
#include "UnitAbilityManager.h"

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


