// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAbility.h"
#include "TBTacticalGameMode.h"

void UUnitAbility::OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef, int IdUnit)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
	UnitAbilityInfos.Add(IdUnit, FUnitAbilityInfoStruct());
	SetAbilityPropertiesOnAssigned(IdUnit);
}

void UUnitAbility::SetAbilityPropertiesOnAssigned(int IdUnit)
{
	SetDynamicRangeValue(IdUnit);
	SetDynamicDamageValue(IdUnit);
}

void UUnitAbility::SetDynamicRangeValue_Implementation(int IdUnit)
{
	UnitAbilityInfos[IdUnit].RangeValue = RangeValue;
}

void UUnitAbility::SetDynamicDamageValue_Implementation(int IdUnit)
{
	UnitAbilityInfos[IdUnit].MinDamage = MinDamage;
	UnitAbilityInfos[IdUnit].MaxDamage = MaxDamage;
}

void UUnitAbility::OnUnitSelected(int IdUnit)
{
	SetTargets(IdUnit);
	for (int i=0; i<UnitAbilityInfos[IdUnit].AllAvailableTargets.Num(); i++)
	{
		SetHitChance(UnitAbilityInfos[IdUnit].AllAvailableTargets[i]);
		SetCritChance(UnitAbilityInfos[IdUnit].AllAvailableTargets[i]);
	}
}

void UUnitAbility::SetTargets_Implementation(int IdUnit)
{
	if (TBTacticalGameMode)
	{
		UnitAbilityInfos[IdUnit].AllAvailableTargets.Empty();
		UnitAbilityInfos[IdUnit].TargetsHitChances.Empty();
		UnitAbilityInfos[IdUnit].TargetsCritChances.Empty();
		
		UnitAbilityInfos[IdUnit].AllAvailableTargets = TBTacticalGameMode->TargetManager->GetAllAvailableTargetsBaseOnAbilityProperties(this);
		for (int i=0; i<UnitAbilityInfos[IdUnit].AllAvailableTargets.Num(); i++)
		{
			AActor* TargetActor = UnitAbilityInfos[IdUnit].AllAvailableTargets[i];
			UnitAbilityInfos[IdUnit].TargetsHitChances.Add(TargetActor, HitChance);
			UnitAbilityInfos[IdUnit].TargetsCritChances.Add(TargetActor, CritChance);
		}
	}
}
void UUnitAbility::SetHitChance_Implementation(AActor* Target)
{
}

void UUnitAbility::SetCritChance_Implementation(AActor* Target)
{
}








