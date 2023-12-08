// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetManager.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "UnitAbility.h"
#include "UnitAbilityManager.h"

void UTargetManager::Initialize(ATBTacticalGameMode* TBTacticalGameModeRef)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
}

void UTargetManager::SelectTarget(int TargetIndex)
{
	if (TargetIndex >= 0 && TargetIndex < AllCurrentAvailableTarget.Num())
	{
		SelectedUnitIndex = TargetIndex;
		AActor* UnitTarget = AllCurrentAvailableTarget[SelectedUnitIndex];
		TBTacticalGameMode->MainController->GoToActor(UnitTarget);
		
		OnTargetSelectedEvent.Broadcast(SelectedUnitIndex);
	}
}

void UTargetManager::SelectNextTarget()
{
	if (SelectedUnitIndex == -1) return;

	SelectedUnitIndex++;
	if (SelectedUnitIndex >= AllCurrentAvailableTarget.Num())
	{
		SelectedUnitIndex = 0;
	}

	SelectTarget(SelectedUnitIndex);
}

void UTargetManager::SelectPreviousTarget()
{
	if (SelectedUnitIndex == -1) return;

	SelectedUnitIndex--;
	if (SelectedUnitIndex < 0)
	{
		SelectedUnitIndex = AllCurrentAvailableTarget.Num()-1;
	}

	SelectTarget(SelectedUnitIndex);
}

AActor* UTargetManager::GetTargetFromIndex(int TargetIndex)
{
	if (TargetIndex >= 0 && TargetIndex < AllCurrentAvailableTarget.Num())
	{
		return AllCurrentAvailableTarget[TargetIndex];
	}
	return nullptr;
}

TArray<AActor*> UTargetManager::GetAllAvailableTargetsBaseOnAbilityProperties(UUnitAbility* UnitAbility)
{
	//UnitAbility->GetOwningActorFromActorInfo() cannot get actor that way unless the ability execute
	
	TArray<AActor*> ReturnedTargets;
	AUnit* CurrentUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
	
	switch (UnitAbility->TargetType)
	{
	case ETargetType::Self:
		ReturnedTargets.Add(CurrentUnit);
		break;
		
	case ETargetType::SingleTarget:
		ReturnedTargets.Append(GetTargetsFromAbiiltyRange(UnitAbility));
		break;
		
	case ETargetType::SpecificGroundPosition:
		
		break;
		
	default:
		break;
	}

	return ReturnedTargets;
}

TArray<AActor*> UTargetManager::GetTargetsFromAbiiltyRange(UUnitAbility* UnitAbility)
{
	//UnitAbility->GetOwningActorFromActorInfo() cannot get actor that way unless the ability execute
	
	AUnit* SeekingUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
	TArray<AActor*> ReturnedTargets;
	
	switch(UnitAbility->AbilityRange)
	{
		case Melee:
			ReturnedTargets = GetTargetsUsingMeleeRange(SeekingUnit, UnitAbility->ValidTargetFaction);
			break;
		case Range:
			ReturnedTargets = GetTargetsInRange(SeekingUnit, UnitAbility->ValidTargetFaction, UnitAbility->GetDynamicRangeValue(SeekingUnit->IdUnit));
			break;
		case RangeLineOfSight:
			ReturnedTargets = GetTargetsInRangeUsingLineOfSight(SeekingUnit, UnitAbility->ValidTargetFaction, UnitAbility->GetDynamicRangeValue(SeekingUnit->IdUnit));
			break;
		default: ;
	}
	
	return ReturnedTargets;
	
}

TArray<AActor*> UTargetManager::GetTargetsInRangeUsingLineOfSight(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFaction>> ValidFactions,
	float LineOfSightRange
	)
{
	TArray<AActor*> ReturnedTargets;
	
	//Get all Unit from the valid factions
	TArray<int> AllValidUnitId;
	for (int i=0; i<ValidFactions.Num(); i++)
	{
		AllValidUnitId.Append(TBTacticalGameMode->UnitManager->GetAllUnitsIdFromFactions(ValidFactions[i]));
	}

	//Validate Line of sight of each potential target
	for (int i=0; i<AllValidUnitId.Num(); i++)
	{
		AUnit* PotentialTarget = TBTacticalGameMode->UnitManager->GetUnitFromId(AllValidUnitId[i]);
		
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(SeekingUnit);
		
		FVector Start = SeekingUnit->GetActorLocation() + SeekingUnit->SightStartingAnchor;
		
		 FVector DeltaToPotentialTarget = (PotentialTarget->GetActorLocation() - Start);
		 DeltaToPotentialTarget.Normalize();
		 FVector End = Start + DeltaToPotentialTarget*LineOfSightRange;

		//GetWorld in UObject has to be from another object that is active in the scene
		DrawDebugLine(TBTacticalGameMode->GetWorld(), Start, End, FColor::Green, false, 10.0f, 0, 1);	
		bool bHit = TBTacticalGameMode->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
		if (bHit)
		{
			if (AUnit* ReturnedUnit = Cast<AUnit>(HitResult.GetActor()))
			{
				if (ReturnedUnit->IdUnit == PotentialTarget->IdUnit)
				{
					ReturnedTargets.Add(Cast<AActor>(PotentialTarget));
					//AllCurrentAvailableTarget.Add();
				}
			}
		}
	}

	return ReturnedTargets;
	
	//TODO: later do the same thing with interactible object when they are available
}

TArray<AActor*> UTargetManager::GetTargetsInRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFaction>> ValidFactions,
	float Range)
{
	TArray<AActor*> ReturnedTargets;
	return ReturnedTargets;
}

TArray<AActor*> UTargetManager::GetTargetsUsingMeleeRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFaction>> ValidFactions
	)
{
	TArray<AActor*> ReturnedTargets;
	return ReturnedTargets;
}
