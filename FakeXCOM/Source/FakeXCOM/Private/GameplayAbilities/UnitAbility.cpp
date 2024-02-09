﻿// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "GameplayAbilities/UnitAbility.h"

#include "TBTacticalGameMode.h"

#include "ActorsObject/Gun.h"
#include "AttributeSets/GunAttributeSet.h"
#include "AttributeSets/UnitAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pathfinder/NodePath.h"
#include "Utility/DebugHeader.h"

void UUnitAbility::OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef, AUnit* Unit)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
	SetAbilityPropertiesOnAssigned(Unit);
}

void UUnitAbility::SetIsDisabled(AUnit* Unit, bool Val)
{
	Unit->UnitAbilityInfos[AbilityId].bIsDisabled = Val;
}

bool UUnitAbility::GetIsDisabled(AUnit* Unit)
{
	return Unit->UnitAbilityInfos[AbilityId].bIsDisabled;
}

void UUnitAbility::SetIsHidden(AUnit* Unit, bool Val)
{
	Unit->UnitAbilityInfos[AbilityId].bIsHidden = Val;
}

bool UUnitAbility::GetIsHidden(AUnit* Unit)
{
	return Unit->UnitAbilityInfos[AbilityId].bIsHidden;
}

void UUnitAbility::SetAbilityEnabledEvent_Implementation(AUnit* Unit)
{
	Unit->UnitAbilityInfos[AbilityId].bIsDisabled = false;
}

void UUnitAbility::SetAbilityHiddenEvent_Implementation(AUnit* Unit)
{
	Unit->UnitAbilityInfos[AbilityId].bIsHidden = false;
}

void UUnitAbility::SetAbilityPropertiesOnAssigned(AUnit* Unit)
{
	if (bUseDynamicRange)
	{
		SetDynamicRangeValueEvent(Unit);
	}
	else
	{
		Unit->UnitAbilityInfos[AbilityId].RangeValue = RangeValue;
	}
}

void UUnitAbility::SetDynamicRangeValueEvent_Implementation(AUnit* Unit)
{
	Unit->UnitAbilityInfos[AbilityId].RangeValue = RangeValue;
}

void UUnitAbility::SetDynamicRangeValue(AUnit* Unit, float NewRangeValue)
{
	Unit->UnitAbilityInfos[AbilityId].RangeValue = NewRangeValue;
}

float UUnitAbility::GetDynamicRangeValue(AUnit* Unit)
{
	return Unit->UnitAbilityInfos[AbilityId].RangeValue;
}

void UUnitAbility::SetDynamicDamageValueEvent_Implementation(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage[TargetUnit->IdUnit] = MinDamage;
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage[TargetUnit->IdUnit] = MaxDamage;
	}
	else
	{
		DebugScreen("UUnitAbility::SetDynamicDamageValueEvent_Implementation : Target is an invalid actor", FColor::Red);
	}
}

void UUnitAbility::SetDynamicDamageValue(AUnit* Unit, AActor* Target, float NewMinDamageValue, float NewMaxDamageValue)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage[TargetUnit->IdUnit] = NewMinDamageValue;
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage[TargetUnit->IdUnit] = NewMaxDamageValue;
	}
	else
	{
		DebugScreen("UUnitAbility::SetDynamicDamageValue : Target is an invalid actor", FColor::Red);
	}
}

float UUnitAbility::GetDynamicMinDamageValue(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		return Unit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage[TargetUnit->IdUnit];
	}
	else
	{
		DebugScreen("UUnitAbility::GetDynamicMinDamageValue : Target is an invalid actor", FColor::Red);
		return -1.0f;
	}
}

float UUnitAbility::GetDynamicMaxDamageValue(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		return Unit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage[TargetUnit->IdUnit];
	}
	else
	{
		DebugScreen("UUnitAbility::GetDynamicMaxDamageValue : Target is an invalid actor", FColor::Red);
		return -1.0f;
	}
}

void UUnitAbility::OnUnitSelected(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	SetTargets(UnitRef);
	
	for (int i=0; i<UnitRef->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets.Num(); i++)
	{
		if (AUnit* TargetUnit = Cast<AUnit>(UnitRef->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets[i]))
		{
			if (bUseDynamicDamage)
			{
				SetDynamicDamageValueEvent(UnitRef, TargetUnit);
			}
			else
			{
				UnitRef->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage[TargetUnit->IdUnit] = MinDamage;
				UnitRef->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage[TargetUnit->IdUnit] = MaxDamage;
			}
		
			if (bUseDynamicHitChance)
			{
				SetHitChanceEvent(UnitRef, TargetUnit);
			}
			else
			{
				UnitRef->UnitAbilityInfos[AbilityId].TargetUnitsHitChances[TargetUnit->IdUnit] = HitChance;
			}

			if (bUseDynamicCritChance)
			{
				SetCritChanceEvent(UnitRef, TargetUnit);
			}
			else
			{
				UnitRef->UnitAbilityInfos[AbilityId].TargetUnitsCritChance[TargetUnit->IdUnit] = CritChance;
			}
		}
	}

	//Sort the target by hit chance
	FUnitAbilityInfoStruct& UnitAbilityInfoRef = UnitRef->UnitAbilityInfos[AbilityId];
	UnitAbilityInfoRef.AllAvailableUnitTargets.Sort([&UnitAbilityInfoRef](const AUnit& A, const AUnit& B)
	{
		const float HitChanceA = UnitAbilityInfoRef.TargetUnitsHitChances.FindRef(A.IdUnit);
		const float HitChanceB = UnitAbilityInfoRef.TargetUnitsHitChances.FindRef(B.IdUnit);
		return HitChanceA > HitChanceB;
	});
}

void UUnitAbility::RotateTowardTarget(AUnit* Unit, AActor* Target)
{
	FVector UnitLocation = Unit->GetActorLocation();
	UnitLocation.Z = 0.0f;

	FVector TargetLocation = Target->GetActorLocation();
	TargetLocation.Z = 0.0f;

	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(UnitLocation, TargetLocation);
	Unit->SetActorRotation(LookAtRotation, ETeleportType::TeleportPhysics);
}

void UUnitAbility::SetTargets_Implementation(AUnit* OwningUnit)
{
	if (GetTBTacticalGameMode())
	{
		
		OwningUnit->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets.Empty();
		OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage.Empty();
		OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage.Empty();
		OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances.Empty();
		OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance.Empty();

		//TODO: Change GetAllAvailableTargetsBaseOnAbilityProperties so it receive reference of list of Unit and fill it. Later could add damageable object
		GetTBTacticalGameMode()->TargetManager->GetAllAvailableTargetsBaseOnAbilityProperties(this, OwningUnit->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets);
		
		for (int i=0; i<OwningUnit->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets.Num(); i++)
		{
			if (const AUnit* TargetUnit = Cast<AUnit>(OwningUnit->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets[i]))
			{
				OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage.Add(TargetUnit->IdUnit, MinDamage);
				OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage.Add(TargetUnit->IdUnit, MaxDamage);
				OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances.Add(TargetUnit->IdUnit, HitChance);
				OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance.Add(TargetUnit->IdUnit, CritChance);
			}
		}

		EndSetTargets(OwningUnit);
	}
}

void UUnitAbility::AddTargets(AUnit* OwningUnit, TArray<int> IdTargetUnits)
{
	if (GetTBTacticalGameMode())
	{
		for (int i=0; i<IdTargetUnits.Num(); i++)
		{
			AUnit* TargetUnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdTargetUnits[i]);
	
			OwningUnit->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets.Add(TargetUnitRef);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage.Add(IdTargetUnits[i], MinDamage);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage.Add(IdTargetUnits[i], MaxDamage);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances.Add(IdTargetUnits[i], HitChance);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance.Add(IdTargetUnits[i], CritChance);
		}
	}
}

void UUnitAbility::RemoveTargets(AUnit* OwningUnit, TArray<int> IdTargetUnits)
{
	if (GetTBTacticalGameMode())
	{
		for (int i=0; i<IdTargetUnits.Num(); i++)
		{
			AUnit* TargetUnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdTargetUnits[i]);
	
			OwningUnit->UnitAbilityInfos[AbilityId].AllAvailableUnitTargets.Remove(TargetUnitRef);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMinDamage.Remove(IdTargetUnits[i]);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsMaxDamage.Remove(IdTargetUnits[i]);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances.Remove(IdTargetUnits[i]);
			OwningUnit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance.Remove(IdTargetUnits[i]);
		}
	}
}

void UUnitAbility::EndSetTargets(AUnit* OwningUnit)
{
	if (GetTBTacticalGameMode())
	{
		SetAbilityEnabledEvent(OwningUnit);
		SetAbilityHiddenEvent(OwningUnit);
	}
}

void UUnitAbility::SetHitChanceEvent_Implementation(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances[TargetUnit->IdUnit] = HitChance;
	}
	else
	{
		DebugScreen("UUnitAbility::SetHitChanceEvent_Implementation : Target is an invalid actor", FColor::Red);
	}
}

float UUnitAbility::GetHitChance_Implementation(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath)
{
	return HitChance;
}

void UUnitAbility::SetHitChance(AUnit* Unit, AActor* Target, float NewHitChanceValue)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances[TargetUnit->IdUnit] = NewHitChanceValue;
	}
	else
	{
		DebugScreen("UUnitAbility::SetHitChanceEvent_Implementation : Target is an invalid actor", FColor::Red);
	}
}

void UUnitAbility::SetCritChanceEvent_Implementation(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance[TargetUnit->IdUnit] = CritChance;
	}
	else
	{
		DebugScreen("UUnitAbility::SetCritChanceEvent_Implementation : Target is an invalid actor", FColor::Red);
	}
}

float UUnitAbility::GetCritChance_Implementation(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath)
{
	return CritChance;
}

void UUnitAbility::SetCritChance(AUnit* Unit , AActor* Target, float NewCritChanceValue)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		Unit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance[TargetUnit->IdUnit] = NewCritChanceValue;
	}
	else
	{
		DebugScreen("UUnitAbility::SetCritChance : Target is an invalid actor", FColor::Red);
	}
}

float UUnitAbility::GetRangeToTarget(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath)
{
	//TODO: in future, might have interactible destructible object
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		const FVector DeltaToTarget =
			(TargetNodePath->GetComponentLocation() + FVector(0.0f, 0.0f, TargetUnit->ZGroundOffset) + TargetUnit->SightStartingAnchor) -
				(UnitNodePath->GetComponentLocation() + FVector(0.0f, 0.0f, Unit->ZGroundOffset) + Unit->SightStartingAnchor);
		return DeltaToTarget.Size();		
	}
	else
	{
		DebugScreen("UUnitAbility::GetRangeToTarget : Target is an invalid actor", FColor::Red);
		return 0.0f;
	}
}

float UUnitAbility::GetTargetHitChance(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		return Unit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances[TargetUnit->IdUnit];
	}
	else
	{
		DebugScreen("UUnitAbility::GetTargetHitChance : Target is an invalid actor", FColor::Red);
		return -1.0f;
	}
}

float UUnitAbility::GetTargetCritChance(AUnit* Unit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		return Unit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance[TargetUnit->IdUnit];
	}
	else
	{
		DebugScreen("UUnitAbility::GetTargetCritChance : Target is an invalid actor", FColor::Red);
		return -1.0f;
	}
}

void UUnitAbility::CostAllActions(AUnit* Unit)
{
	Unit->UnitAttributeSet->SetActions(0.0f);
}

void UUnitAbility::CostActions(AUnit* Unit, float CostValue)
{
	const UUnitAttributeSet* TargetUnitAttributeSet = Unit->UnitAttributeSet;
	const float NewActionsValue = TargetUnitAttributeSet->GetActions() - CostValue;
	TargetUnitAttributeSet->SetActions(NewActionsValue);
}

void UUnitAbility::CostAmmo(AGun* Gun, float CostValue)
{
	const UGunAttributeSet* OwnedGunAttributeSet = Gun->GunAttributeSet;
	const float NewAmmosValue = OwnedGunAttributeSet->GetAmmo() - CostValue;
	OwnedGunAttributeSet->SetAmmo(NewAmmosValue);
}

void UUnitAbility::RechargeAllAmmo(AGun* Gun)
{
	const UGunAttributeSet* OwnedGunAttributeSet = Gun->GunAttributeSet;
	const float NewAmmosValue = OwnedGunAttributeSet->GetMaxAmmo();
	OwnedGunAttributeSet->SetAmmo(NewAmmosValue);
}

void UUnitAbility::ApplyDamage(AActor* Target, float DamageValue, const bool IsCrit)
{
	if (AUnit* UnitTarget = Cast<AUnit>(Target))
	{
		const UUnitAttributeSet* TargetUnitAttributeSet = UnitTarget->UnitAttributeSet;
		const float NewHealth = TargetUnitAttributeSet->GetHealth() - DamageValue;
		TargetUnitAttributeSet->SetHealth(NewHealth);

		if (IsCrit)
		{
			GetTBTacticalGameMode()->LevelUIRef->CallStatusEvent(UnitTarget, EStatusType::CriticalDamage, DamageValue);
		}
		else
		{
			GetTBTacticalGameMode()->LevelUIRef->CallStatusEvent(UnitTarget, EStatusType::Damage, DamageValue);
		}
	}
}

void UUnitAbility::ApplyHeal(AActor* Target, float DamageValue, bool IsCrit)
{
	if (AUnit* UnitTarget = Cast<AUnit>(Target))
	{
		const UUnitAttributeSet* TargetUnitAttributeSet = UnitTarget->UnitAttributeSet;
		const float NewHealth = TargetUnitAttributeSet->GetHealth() + DamageValue;
		TargetUnitAttributeSet->SetHealth(NewHealth);

		if (IsCrit)
		{
			GetTBTacticalGameMode()->LevelUIRef->CallStatusEvent(UnitTarget, EStatusType::CriticalHealing, DamageValue);
		}
		else
		{
			GetTBTacticalGameMode()->LevelUIRef->CallStatusEvent(UnitTarget, EStatusType::Healing, DamageValue);
		}
	}
}

bool UUnitAbility::RollDiceForHit(AUnit* UnitTryingToHit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		const float CurrentHitChance = UnitTryingToHit->UnitAbilityInfos[AbilityId].TargetUnitsHitChances[TargetUnit->IdUnit];
		const float DiceRoll = FMath::RandRange(0.0f, 100.0f);
		return CurrentHitChance > DiceRoll;
	}
	else
	{
		DebugScreen("UUnitAbility::RollDiceForHit : Target is an invalid actor", FColor::Red);
		return false;
	}
}

bool UUnitAbility::RollDiceForCrit(AUnit* UnitTryingToHit, AActor* Target)
{
	if (const AUnit* TargetUnit = Cast<AUnit>(Target))
	{
		const float CurrentCritChance = UnitTryingToHit->UnitAbilityInfos[AbilityId].TargetUnitsCritChance[TargetUnit->IdUnit];
		const float DiceRoll = FMath::RandRange(0.0f, 100.0f);
		return CurrentCritChance > DiceRoll;
	}
	else
	{
		DebugScreen("UUnitAbility::RollDiceForCrit : Target is an invalid actor", FColor::Red);
		return false;
	}
}

ATBTacticalGameMode* UUnitAbility::GetTBTacticalGameMode()
{
	if (!TBTacticalGameMode)
	{
		return GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	}
	else
	{
		return TBTacticalGameMode;
	}
}





