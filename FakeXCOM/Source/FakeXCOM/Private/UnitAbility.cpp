// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAbility.h"
#include "TBTacticalGameMode.h"

void UUnitAbility::OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
	SetAbilityPropertiesOnAssigned();
}

void UUnitAbility::SetTargets_Implementation()
{
	if (TBTacticalGameMode)
	{
		AllAvailableTarget.Empty();
		AllAvailableTarget = TBTacticalGameMode->TargetManager->GetAllAvailableTargetsBaseOnAbilityProperties(this);	
	}
}

void UUnitAbility::SetAbilityPropertiesOnAssigned_Implementation()
{
}

void UUnitAbility::OnTargetSelected(int TargetIndex)
{
	SetAbilityPropertiesBaseOnTargetSelected(TBTacticalGameMode->TargetManager->GetTargetFromIndex(TargetIndex));
}

void UUnitAbility::SetAbilityPropertiesBaseOnTargetSelected_Implementation(AActor* TargetActor)
{
}


