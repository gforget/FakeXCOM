// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetManager.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "UnitAbility.h"

void UTargetManager::Initialize(ATBTacticalGameMode* TBTacticalGameModeRef)
{
	TBTacticalGameMode = TBTacticalGameModeRef;
}

void UTargetManager::SelectTarget(int TargetIndex)
{
	if (TargetIndex >= 0 && TargetIndex < AllCurrentAvailableTarget.Num())
	{
		SelectedTargetIndex = TargetIndex;
		AActor* UnitTarget = AllCurrentAvailableTarget[SelectedTargetIndex];
		TBTacticalGameMode->MainController->GoToActor(UnitTarget);
		
		OnTargetSelectedEvent.Broadcast(SelectedTargetIndex);
	}
}

AActor* UTargetManager::GetCurrentlySelectedTarget()
{
	return AllCurrentAvailableTarget[SelectedTargetIndex];
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
	AUnit* SeekingUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
	TArray<AActor*> ReturnedTargets;
	
	switch(UnitAbility->AbilityRange)
	{
		case Melee:
			ReturnedTargets = GetTargetsUsingMeleeRange(SeekingUnit,
				UnitAbility->ValidTargetFactionRelation,
				UnitAbility->DeadTargetFilter);
			break;
		case Range:
			ReturnedTargets = GetTargetsInRange(SeekingUnit,
				UnitAbility->ValidTargetFactionRelation,
				UnitAbility->GetDynamicRangeValue(SeekingUnit->IdUnit),
				UnitAbility->DeadTargetFilter);
			break;
		case RangeLineOfSight:
			ReturnedTargets = GetTargetsInRangeUsingLineOfSight(SeekingUnit,
				UnitAbility->ValidTargetFactionRelation,
				UnitAbility->GetDynamicRangeValue(SeekingUnit->IdUnit),
				UnitAbility->DeadTargetFilter);
			break;
		default: ;
	}
	
	return ReturnedTargets;
	
}

TArray<AActor*> UTargetManager::GetTargetsInRangeUsingLineOfSight(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
	float LineOfSightRange,
	TEnumAsByte<EDeadTargetFilter> DeadTargetFilter
	)
{
	TArray<AActor*> ReturnedTargets;
	
	//Get all Unit from the valid factions
	TArray<EFaction> AllValidFaction;
	for (int i=0; i<ValidFactionsRelation.Num(); i++)
	{
		UClass* OuterClass = UFactionManager::StaticClass();
		if (UEnum* EnumPtr = FindObject<UEnum>(OuterClass, TEXT("EFaction"), true))
		{
			for (int32 EnumIndex = 0; EnumIndex < EnumPtr->NumEnums(); EnumIndex++)
			{
				EFaction CurrentFactionValue = static_cast<EFaction>(EnumPtr->GetValueByIndex(EnumIndex));
				if (TBTacticalGameMode->FactionManagerComponent->GetFactionRelation(SeekingUnit->Faction, CurrentFactionValue) == ValidFactionsRelation[i])
				{
					AllValidFaction.Add(CurrentFactionValue);
				}
			}
		}
	}
	
	TArray<int> AllValidUnitId;
	for (int i=0; i<AllValidFaction.Num(); i++)
	{
		AllValidUnitId.Append(TBTacticalGameMode->UnitManager->GetAllUnitsIdFromFactions(AllValidFaction[i]));
	}
	
	//Validate Line of sight of each potential target
	for (int i=0; i<AllValidUnitId.Num(); i++)
	{
		AUnit* PotentialTarget = TBTacticalGameMode->UnitManager->GetUnitFromId(AllValidUnitId[i]);

		switch(DeadTargetFilter)
		{
			case EDeadTargetFilter::NoDeadTarget:
				if (PotentialTarget->GetIsDead())
				{
					continue;
				} 
			break;

			case EDeadTargetFilter::OnlyDeadTarget:
				if (!PotentialTarget->GetIsDead())
				{
					continue;
				}
			break;
			default: ;
		}

		if (ConfirmLineOfSightOnUnit(SeekingUnit,
			PotentialTarget,
			SeekingUnit->GetActorLocation(),
			PotentialTarget->GetActorLocation(),
			LineOfSightRange
			))
		{
			ReturnedTargets.Add(Cast<AActor>(PotentialTarget));
		}
	}

	return ReturnedTargets;
	
	//TODO: later do the same thing with interactible object when they are available
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

TArray<AActor*> UTargetManager::GetTargetsInRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
	float Range,
	TEnumAsByte<EDeadTargetFilter> DeadTargetFilter)
{
	TArray<AActor*> ReturnedTargets;
	return ReturnedTargets;
}

TArray<AActor*> UTargetManager::GetTargetsUsingMeleeRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
	TEnumAsByte<EDeadTargetFilter> DeadTargetFilter
	)
{
	TArray<AActor*> ReturnedTargets;
	return ReturnedTargets;
}
