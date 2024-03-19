// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "Managers/TargetManager.h"

#include "TBTacticalGameMode.h"

#include "Controller/TBTacticalMainController.h"
#include "GameplayAbilities/UnitAbility.h"
#include "Utility/DebugHeader.h"

void UTargetManager::Initialize(ATBTacticalGameMode* TBTacticalGameModeRef)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
}

void UTargetManager::SelectTarget(int TargetIndex, bool bMoveToActor)
{
	if (TargetIndex >= 0 && TargetIndex < AllCurrentAvailableTarget.Num())
	{
		SelectedTargetIndex = TargetIndex;
		AActor* UnitTarget = AllCurrentAvailableTarget[SelectedTargetIndex];
		if (bMoveToActor)
		{
			TBTacticalGameMode->MainController->GoToActor(UnitTarget);
		}
		
		OnTargetSelectedEvent.Broadcast(SelectedTargetIndex, TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit());
	}
}

AActor* UTargetManager::GetCurrentlySelectedTarget()
{
	if (SelectedTargetIndex != -1)
	{
		return AllCurrentAvailableTarget[SelectedTargetIndex];
	}
	
	DebugScreen("There is no currently selected target", FColor::Red);
	return nullptr;
}

void UTargetManager::SelectNextTarget()
{
	if (SelectedTargetIndex == -1) return;

	SelectedTargetIndex++;
	if (SelectedTargetIndex >= AllCurrentAvailableTarget.Num())
	{
		SelectedTargetIndex = 0;
	}

	SelectTarget(SelectedTargetIndex);
}

void UTargetManager::SelectPreviousTarget()
{
	if (SelectedTargetIndex == -1) return;

	SelectedTargetIndex--;
	if (SelectedTargetIndex < 0)
	{
		SelectedTargetIndex = AllCurrentAvailableTarget.Num()-1;
	}

	SelectTarget(SelectedTargetIndex);
}

void UTargetManager::UpdateAllCurrentAvailableTargetWithAbilityTargets(AUnit* OwningUnit, const FString& UnitAbilityId)
{
	//TODO: will add other type of target later
	TBTacticalGameMode->TargetManager->AllCurrentAvailableTarget.Empty();
	TArray<AUnit*> AllTargetUnit = OwningUnit->UnitAbilityInfos[UnitAbilityId].AllAvailableUnitTargets;
	for (int i=0; i<AllTargetUnit.Num(); i++)
	{
		TBTacticalGameMode->TargetManager->AllCurrentAvailableTarget.Add(Cast<AActor>(AllTargetUnit[i]));
	}
}

AActor* UTargetManager::GetTargetFromIndex(int TargetIndex)
{
	if (TargetIndex >= 0 && TargetIndex < AllCurrentAvailableTarget.Num())
	{
		return AllCurrentAvailableTarget[TargetIndex];
	}
	return nullptr;
}

void UTargetManager::GetAllAvailableTargetsBaseOnAbilityProperties(UUnitAbility* UnitAbility, TArray<AUnit*>& AllAvailableUnitTargets)
{
	AUnit* CurrentUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
	
	switch (UnitAbility->TargetType)
	{
	case ETargetType::Self:
		AllAvailableUnitTargets.Add(CurrentUnit);
		break;
		
	case ETargetType::SingleTarget:
		GetTargetsFromAbiiltyRange(UnitAbility, AllAvailableUnitTargets);
		break;
		
	case ETargetType::SpecificGroundPosition:
		
		break;
		
	default:
		break;
	}
}

void UTargetManager::GetTargetsFromAbiiltyRange(UUnitAbility* UnitAbility, TArray<AUnit*>& AllAvailableUnitTargets)
{
	AUnit* SeekingUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
	
	switch(UnitAbility->AbilityRange)
	{
		case Melee:
			GetTargetsUsingMeleeRange(SeekingUnit,
				UnitAbility,
				AllAvailableUnitTargets);
			break;
		case Range:
			GetTargetsInRange(SeekingUnit,
				UnitAbility,
				AllAvailableUnitTargets);
			break;
		case RangeLineOfSight:
			GetTargetsInRangeUsingLineOfSight(SeekingUnit,
				UnitAbility,
				AllAvailableUnitTargets);
			break;
		default: ;
	}
}

void UTargetManager::GetTargetsInRangeUsingLineOfSight(
	AUnit* SeekingUnit,
	UUnitAbility* UnitAbility,
	TArray<AUnit*>& AllAvailableUnitTargets
	)
{
	TArray<int> AllValidUnitId = GetAllValidUnitIdFromFactionRelation(SeekingUnit, UnitAbility->ValidTargetFactionRelation);
	
	//Validate Line of sight of each potential target
	for (int i=0; i<AllValidUnitId.Num(); i++)
	{
		AUnit* PotentialTarget = TBTacticalGameMode->UnitManager->GetUnitFromId(AllValidUnitId[i]);
		if (!UnitAbility->FilterTargets(SeekingUnit, PotentialTarget))
		{
			continue;	
		}
		
		if (!ValidateTargetDeathFilter(PotentialTarget, UnitAbility->DeadTargetFilter))
		{
			continue;
		}
		
		if (ConfirmLineOfSightOnUnit(SeekingUnit,
			PotentialTarget,
			SeekingUnit->GetActorLocation(),
			PotentialTarget->GetActorLocation(),
			UnitAbility->GetRangeValue(SeekingUnit)
			))
		{
			AllAvailableUnitTargets.Add(PotentialTarget);
		}
	}
}

TArray<int> UTargetManager::GetAllValidUnitIdFromFactionRelation(AUnit* SeekingUnit, TArray<TEnumAsByte<EFactionRelation>> ValidRelations)
{
	TArray<EFaction> AllValidFaction;
	for (int i=0; i<ValidRelations.Num(); i++)
	{
		const TArray<EFaction> AllFaction = TBTacticalGameMode->FactionManagerComponent->AllFaction;
		for (int j=0; j<AllFaction.Num(); j++)
		{
			EFaction CurrentFactionValue = AllFaction[j];
			if (TBTacticalGameMode->FactionManagerComponent->GetFactionRelation(SeekingUnit->Faction, CurrentFactionValue) == ValidRelations[i])
			{
				AllValidFaction.Add(CurrentFactionValue);
			}
		}
	}
	
	TArray<int> AllValidUnitId;
	for (int i=0; i<AllValidFaction.Num(); i++)
	{
		AllValidUnitId.Append(TBTacticalGameMode->UnitManager->GetAllUnitsIdFromFactions(AllValidFaction[i]));
	}

	return AllValidUnitId;
}

bool UTargetManager::ValidateTargetDeathFilter(AUnit* PotentialTarget, TEnumAsByte<EDeadTargetFilter> DeadTargetFilter)
{
	switch(DeadTargetFilter)
	{
		case EDeadTargetFilter::NoDeadTarget:
			if (PotentialTarget->GetIsDead())
			{
				return false;
			} 
			break;
		
		case EDeadTargetFilter::OnlyDeadTarget:
			if (!PotentialTarget->GetIsDead())
			{
				return false;
			}
			break;
		default: ;
	}

	return true;
}

bool UTargetManager::ConfirmLineOfSightOnUnit(
	AUnit* SeekingUnit,
	AUnit* TargetUnit,
	FVector SeekingUnitPosition,
	FVector TargetUnitPosition,
	float LineOfSightRange)
{
	for (int i=0; i<TargetUnit->SightSurroundDefendingAnchor.Num(); i++)
	{
		//To validate if you have line of sight, the potential target has surround anchor so if the target hide behind an obstacle measuring one cube
		//you should still be able to see it from the side. Since those anchor do not have collider, we verify if the linecast has pass through
		//the target by measuring the line of sight compare to the actual distance to the target.
			
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(SeekingUnit);
		CollisionParams.AddIgnoredActor(TargetUnit);
			
		FVector Start = SeekingUnitPosition + SeekingUnit->SightSurroundTargetingAnchor[i];
		FVector DeltaToPotentialTarget = ((TargetUnitPosition + TargetUnit->SightSurroundDefendingAnchor[i]) - Start);
			
		FVector DeltaToPotentialTargetNormalized = DeltaToPotentialTarget;
		DeltaToPotentialTargetNormalized.Normalize();
			
		FVector End = Start + DeltaToPotentialTargetNormalized*LineOfSightRange;
			
		//GetWorld in UObject has to be called from another object that is active in the scene

		bool bHit = TBTacticalGameMode->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
		const bool bDebugTrace = false;
		if (bDebugTrace)
		{
			FVector End2 = End;
			FColor LineColor = FColor::Green;
			if (bHit)
			{
				End2 = HitResult.Location;
				LineColor = FColor::Red;
			}
			DrawDebugLine(TBTacticalGameMode->GetWorld(), Start, End2, LineColor, false, 10.0f, 0, 1);	
		}
			
		FVector DeltaToHit = HitResult.Location - Start;
		if ((bHit && DeltaToPotentialTarget.Size() <= DeltaToHit.Size()) || // hit something, but verify if it passed through the target
		(!bHit && DeltaToPotentialTarget.Size() <= LineOfSightRange)) // didn't hit anything, but verify if the line of sight was long enough to reach the target
			{
			return true;
			}
	}

	return false;
}

void UTargetManager::GetTargetsInRange(
	AUnit* SeekingUnit,
	UUnitAbility* UnitAbility,
	TArray<AUnit*>& AllAvailableUnitTargets)
{
	//TODO: Feed AllAvailableUnitTargets
}

void UTargetManager::GetTargetsUsingMeleeRange(
	AUnit* SeekingUnit,
	UUnitAbility* UnitAbility,
	TArray<AUnit*>& AllAvailableUnitTargets
	)
{
	//TODO: Feed AllAvailableUnitTargets
}
