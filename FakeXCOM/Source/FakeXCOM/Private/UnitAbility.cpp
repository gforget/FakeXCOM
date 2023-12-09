// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAbility.h"
#include "TBTacticalGameMode.h"

void UUnitAbility::OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef, int IdUnit)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
	UnitAbilityInfos.Add(IdUnit, FUnitAbilityInfoStruct());
	SetAbilityPropertiesOnAssigned(IdUnit);
}

void UUnitAbility::SetIsDisabled(AUnit* Unit, bool Val)
{
	UnitAbilityInfos[Unit->IdUnit].bIsDisabled = Val;
}

bool UUnitAbility::GetIsDisabled(AUnit* Unit)
{
	return UnitAbilityInfos[Unit->IdUnit].bIsDisabled;
}

void UUnitAbility::SetIsHidden(AUnit* Unit, bool Val)
{
	UnitAbilityInfos[Unit->IdUnit].bIsHidden = Val;
}

bool UUnitAbility::GetIsHidden(AUnit* Unit)
{
	return UnitAbilityInfos[Unit->IdUnit].bIsHidden;
}

void UUnitAbility::SetAbilityEnabledEvent_Implementation(AUnit* Unit)
{
	UnitAbilityInfos[Unit->IdUnit].bIsDisabled = false;
}

void UUnitAbility::SetAbilityHiddenEvent_Implementation(AUnit* Unit)
{
	UnitAbilityInfos[Unit->IdUnit].bIsHidden = false;
}

void UUnitAbility::SetAbilityPropertiesOnAssigned(int IdUnit)
{
	SetDynamicRangeValueEvent(TBTacticalGameMode->UnitManager->GetUnitFromId(IdUnit));
	SetDynamicDamageValueEvent(TBTacticalGameMode->UnitManager->GetUnitFromId(IdUnit));
}

void UUnitAbility::SetDynamicRangeValueEvent_Implementation(AUnit* Unit)
{
	UnitAbilityInfos[Unit->IdUnit].RangeValue = RangeValue;
}

void UUnitAbility::SetDynamicRangeValue(int IdUnit, float NewRangeValue)
{
	UnitAbilityInfos[IdUnit].RangeValue = NewRangeValue;
}

float UUnitAbility::GetDynamicRangeValue(int IdUnit)
{
	return UnitAbilityInfos[IdUnit].RangeValue;
}

void UUnitAbility::SetDynamicDamageValueEvent_Implementation(AUnit* Unit)
{
	UnitAbilityInfos[Unit->IdUnit].MinDamage = MinDamage;
	UnitAbilityInfos[Unit->IdUnit].MaxDamage = MaxDamage;
}

void UUnitAbility::SetDynamicDamageValue(int IdUnit, float NewMinDamageValue, float NewMaxDamageValue)
{
	UnitAbilityInfos[IdUnit].MinDamage = NewMinDamageValue;
	UnitAbilityInfos[IdUnit].MaxDamage = NewMaxDamageValue;
}

float UUnitAbility::GetDynamicMinDamageValue(int IdUnit)
{
	return UnitAbilityInfos[IdUnit].MinDamage;
}

float UUnitAbility::GetDynamicMaxDamageValue(int IdUnit)
{
	return UnitAbilityInfos[IdUnit].MaxDamage;
}

void UUnitAbility::OnUnitSelected(int IdUnit)
{
	SetTargets(IdUnit);
	for (int i=0; i<UnitAbilityInfos[IdUnit].AllAvailableTargets.Num(); i++)
	{
		SetHitChanceEvent(TBTacticalGameMode->UnitManager->GetUnitFromId(IdUnit), UnitAbilityInfos[IdUnit].AllAvailableTargets[i]);
		SetCritChanceEvent(TBTacticalGameMode->UnitManager->GetUnitFromId(IdUnit), UnitAbilityInfos[IdUnit].AllAvailableTargets[i]);
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
		
		SetAbilityEnabledEvent(TBTacticalGameMode->UnitManager->GetUnitFromId(IdUnit));
		SetAbilityHiddenEvent(TBTacticalGameMode->UnitManager->GetUnitFromId(IdUnit));
	}
}

void UUnitAbility::SetHitChanceEvent_Implementation(AUnit* Unit, AActor* Target)
{
}

void UUnitAbility::SetHitChance(int IdUnit, AActor* Target, float NewHitChanceValue)
{
	UnitAbilityInfos[IdUnit].TargetsHitChances[Target] = NewHitChanceValue;
}

void UUnitAbility::SetCritChanceEvent_Implementation(AUnit* Unit, AActor* Target)
{
}

void UUnitAbility::SetCritChance(int IdUnit, AActor* Target, float NewCritChanceValue)
{
	UnitAbilityInfos[IdUnit].TargetsCritChances[Target] = NewCritChanceValue;
}

float UUnitAbility::GetRangeToTarget(AUnit* Unit, AActor* Target)
{
	//TODO: in future, might have interactible destructible object
	if (const AUnit* UnitTarget = Cast<AUnit>(Target))
	{
		const FVector DeltaToTarget = (UnitTarget->GetActorLocation() + UnitTarget->SightStartingAnchor) - (Unit->GetActorLocation() + Unit->SightStartingAnchor);
		return DeltaToTarget.Size();		
	}
	
	return 0.0f;
}

float UUnitAbility::GetTargetHitChance(int IdUnit, AActor* Target)
{
	return UnitAbilityInfos[IdUnit].TargetsHitChances[Target];
}

float UUnitAbility::GetTargetCritChance(int IdUnit, AActor* Target)
{
	return UnitAbilityInfos[IdUnit].TargetsCritChances[Target];
}






