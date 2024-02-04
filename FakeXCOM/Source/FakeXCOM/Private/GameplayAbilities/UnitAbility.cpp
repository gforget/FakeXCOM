// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "GameplayAbilities/UnitAbility.h"

#include "TBTacticalGameMode.h"

#include "ActorsObject/Gun.h"
#include "AttributeSets/GunAttributeSet.h"
#include "AttributeSets/UnitAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pathfinder/NodePath.h"

void UUnitAbility::OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef, int IdUnit)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
	SetAbilityPropertiesOnAssigned(IdUnit);
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

void UUnitAbility::SetAbilityPropertiesOnAssigned(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	if (bUseDynamicRange)
	{
		SetDynamicRangeValueEvent(UnitRef);
	}
	else
	{
		UnitRef->UnitAbilityInfos[AbilityId].RangeValue = RangeValue;
	}
	
	if (bUseDynamicDamage)
	{
		SetDynamicDamageValueEvent(UnitRef);
	}
	else
	{
		UnitRef->UnitAbilityInfos[AbilityId].MinDamage = MinDamage;
		UnitRef->UnitAbilityInfos[AbilityId].MaxDamage = MaxDamage;
	}
}

void UUnitAbility::SetDynamicRangeValueEvent_Implementation(AUnit* Unit)
{
	Unit->UnitAbilityInfos[AbilityId].RangeValue = RangeValue;
}

void UUnitAbility::SetDynamicRangeValue(int IdUnit, float NewRangeValue)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	UnitRef->UnitAbilityInfos[AbilityId].RangeValue = NewRangeValue;
}

float UUnitAbility::GetDynamicRangeValue(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	return UnitRef->UnitAbilityInfos[AbilityId].RangeValue;
}

void UUnitAbility::SetDynamicDamageValueEvent_Implementation(AUnit* Unit)
{
	Unit->UnitAbilityInfos[AbilityId].MinDamage = MinDamage;
	Unit->UnitAbilityInfos[AbilityId].MaxDamage = MaxDamage;
}

void UUnitAbility::SetDynamicDamageValue(int IdUnit, float NewMinDamageValue, float NewMaxDamageValue)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	UnitRef->UnitAbilityInfos[AbilityId].MinDamage = NewMinDamageValue;
	UnitRef->UnitAbilityInfos[AbilityId].MaxDamage = NewMaxDamageValue;
}

float UUnitAbility::GetDynamicMinDamageValue(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	return UnitRef->UnitAbilityInfos[AbilityId].MinDamage;
}

float UUnitAbility::GetDynamicMaxDamageValue(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	return UnitRef->UnitAbilityInfos[AbilityId].MaxDamage;
}

void UUnitAbility::OnUnitSelected(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	
	SetTargets(IdUnit);
	for (int i=0; i<UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets.Num(); i++)
	{
		if (bUseDynamicHitChance)
		{
			SetHitChanceEvent(GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit), UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets[i]);
		}
		else
		{
			UnitRef->UnitAbilityInfos[AbilityId].TargetsHitChances[UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets[i]] = HitChance;
		}

		if (bUseDynamicCritChance)
		{
			SetCritChanceEvent(GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit), UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets[i]);
		}
		else
		{
			UnitRef->UnitAbilityInfos[AbilityId].TargetsCritChances[UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets[i]] = CritChance;
		}
	}

	//Sort the target by hit chance
	FUnitAbilityInfoStruct& UnitAbilityInfoRef = UnitRef->UnitAbilityInfos[AbilityId];
	UnitAbilityInfoRef.AllAvailableTargets.Sort([&UnitAbilityInfoRef](const AActor& A, const AActor& B)
	{
		const float HitChanceA = UnitAbilityInfoRef.TargetsHitChances.FindRef(&A);
		const float HitChanceB = UnitAbilityInfoRef.TargetsHitChances.FindRef(&B);
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

void UUnitAbility::SetTargets_Implementation(int IdUnit)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	if (GetTBTacticalGameMode())
	{
		UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets.Empty();
		UnitRef->UnitAbilityInfos[AbilityId].TargetsHitChances.Empty();
		UnitRef->UnitAbilityInfos[AbilityId].TargetsCritChances.Empty();
		
		UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets = GetTBTacticalGameMode()->TargetManager->GetAllAvailableTargetsBaseOnAbilityProperties(this);
		for (int i=0; i<UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets.Num(); i++)
		{
			AActor* TargetActor = UnitRef->UnitAbilityInfos[AbilityId].AllAvailableTargets[i];
			UnitRef->UnitAbilityInfos[AbilityId].TargetsHitChances.Add(TargetActor, HitChance);
			UnitRef->UnitAbilityInfos[AbilityId].TargetsCritChances.Add(TargetActor, CritChance);
		}
		
		SetAbilityEnabledEvent(GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit));
		SetAbilityHiddenEvent(GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit));
	}
}

void UUnitAbility::SetHitChanceEvent_Implementation(AUnit* Unit, AActor* Target)
{
	Unit->UnitAbilityInfos[AbilityId].TargetsHitChances[Target] = HitChance;
}

float UUnitAbility::GetHitChance_Implementation(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath)
{
	return HitChance;
}

void UUnitAbility::SetHitChance(int IdUnit, AActor* Target, float NewHitChanceValue)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	UnitRef->UnitAbilityInfos[AbilityId].TargetsHitChances[Target] = NewHitChanceValue;
}

void UUnitAbility::SetCritChanceEvent_Implementation(AUnit* Unit, AActor* Target)
{
	Unit->UnitAbilityInfos[AbilityId].TargetsCritChances[Target] = CritChance;
}

float UUnitAbility::GetCritChance_Implementation(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath)
{
	return CritChance;
}

void UUnitAbility::SetCritChance(int IdUnit, AActor* Target, float NewCritChanceValue)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	UnitRef->UnitAbilityInfos[AbilityId].TargetsCritChances[Target] = NewCritChanceValue;
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
	
	return 0.0f;
}

float UUnitAbility::GetTargetHitChance(int IdUnit, AActor* Target)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	return UnitRef->UnitAbilityInfos[AbilityId].TargetsHitChances[Target];
}

float UUnitAbility::GetTargetCritChance(int IdUnit, AActor* Target)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit);
	return UnitRef->UnitAbilityInfos[AbilityId].TargetsCritChances[Target];
}

void UUnitAbility::CostAllActions(int IdUnit)
{
	if (const AUnit* Unit = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit))
	{
		Unit->UnitAttributeSet->SetActions(0.0f);
	}
}

void UUnitAbility::CostActions(int IdUnit, float CostValue)
{
	if (const AUnit* Unit = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnit))
	{
		const UUnitAttributeSet* TargetUnitAttributeSet = Unit->UnitAttributeSet;
		const float NewActionsValue = TargetUnitAttributeSet->GetActions() - CostValue;
		TargetUnitAttributeSet->SetActions(NewActionsValue);
	}
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

bool UUnitAbility::RollDiceForHit(int IdUnitTryingToHit, AActor* Target)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnitTryingToHit);
	const float CurrentHitChance = UnitRef->UnitAbilityInfos[AbilityId].TargetsHitChances[Target];
	const float DiceRoll = FMath::RandRange(0.0f, 100.0f);
	return CurrentHitChance > DiceRoll;
}

bool UUnitAbility::RollDiceForCrit(int IdUnitTryingToHit, AActor* Target)
{
	AUnit* UnitRef = GetTBTacticalGameMode()->UnitManager->GetUnitFromId(IdUnitTryingToHit);
	const float CurrentCritChance = UnitRef->UnitAbilityInfos[AbilityId].TargetsCritChances[Target];
	const float DiceRoll = FMath::RandRange(0.0f, 100.0f);
	return CurrentCritChance > DiceRoll;
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





