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

void UTargetManager::SelectNextTarget()
{
	if (SelectedUnitIndex == -1) return;

	SelectedUnitIndex++;
	if (SelectedUnitIndex > AllAvailableTarget.Num())
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
		SelectedUnitIndex = AllAvailableTarget.Num()-1;
	}

	SelectTarget(SelectedUnitIndex);
}

AActor* UTargetManager::GetTargetFromIndex(int TargetIndex)
{
	if (TargetIndex >= 0 && TargetIndex < AllAvailableTarget.Num())
	{
		return AllAvailableTarget[TargetIndex];
	}
	return nullptr;
}

void UTargetManager::SelectTarget(int TargetIndex)
{
	if (TargetIndex >= 0 && TargetIndex < AllAvailableTarget.Num())
	{
		SelectedUnitIndex = TargetIndex;
		AActor* UnitTarget = AllAvailableTarget[SelectedUnitIndex];
		TBTacticalGameMode->MainController->GoToActor(UnitTarget);
		
		OnTargetSelectedEvent.Broadcast(SelectedUnitIndex);
		
		//for some reason, can't bind event through add dynamic in a UAbility
		TBTacticalGameMode->UnitAbilityManager->CurrentSelectedAbility->OnTargetSelected(SelectedUnitIndex);
	}
}

void UTargetManager::UpdateTargetsUsingLineOfSight(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFaction>> ValidFactions,
	float LineOfSightRange
	)
{
	//Reset Collection
	AllAvailableTarget.Empty();
	
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
					AllAvailableTarget.Add(Cast<AActor>(PotentialTarget));
				}
			}
		}
	}
	
	//TODO: later do the same thing with interactible object when they are available
}

void UTargetManager::UpdateTargetsUsingMeleeRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFaction>> ValidFactions
	)
{
	
}
