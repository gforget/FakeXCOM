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
	AUnit* SeekingUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
	TArray<AActor*> ReturnedTargets;
	
	switch(UnitAbility->AbilityRange)
	{
		case Melee:
			ReturnedTargets = GetTargetsUsingMeleeRange(SeekingUnit, UnitAbility->ValidTargetFactionRelation);
			break;
		case Range:
			ReturnedTargets = GetTargetsInRange(SeekingUnit, UnitAbility->ValidTargetFactionRelation, UnitAbility->GetDynamicRangeValue(SeekingUnit->IdUnit));
			break;
		case RangeLineOfSight:
			ReturnedTargets = GetTargetsInRangeUsingLineOfSight(SeekingUnit, UnitAbility->ValidTargetFactionRelation, UnitAbility->GetDynamicRangeValue(SeekingUnit->IdUnit));
			break;
		default: ;
	}
	
	return ReturnedTargets;
	
}

TArray<AActor*> UTargetManager::GetTargetsInRangeUsingLineOfSight(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
	float LineOfSightRange
	)
{
	TArray<AActor*> ReturnedTargets;
	
	//Get all Unit from the valid factions
	TArray<EFaction> AllValidFaction;
	for (int i=0; i<ValidFactionsRelation.Num(); i++)
	{
		if (UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFaction"), true))
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
		for (int j=0; j<PotentialTarget->SightSurroundAnchor.Num(); j++)
		{
			//To validate if you have line of sight, the potential target has surround anchor so if the target hide behind an obstacle measuring one cube
			//you should still be able to see it from the side. Since those anchor do not have collider, we verify if the linecast has pass through
			//the target by measuring the line of sight compare to the actual distance to the target.
			
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(SeekingUnit);
			CollisionParams.AddIgnoredActor(PotentialTarget);
			
			FVector Start = SeekingUnit->GetActorLocation() + SeekingUnit->SightStartingAnchor;

			FVector DeltaToPotentialTarget = ((PotentialTarget->GetActorLocation() + PotentialTarget->SightSurroundAnchor[j]) - Start);
			
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
				ReturnedTargets.Add(Cast<AActor>(PotentialTarget));
				break;
			}
		}
	}

	return ReturnedTargets;
	
	//TODO: later do the same thing with interactible object when they are available
}

TArray<AActor*> UTargetManager::GetTargetsInRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
	float Range)
{
	TArray<AActor*> ReturnedTargets;
	return ReturnedTargets;
}

TArray<AActor*> UTargetManager::GetTargetsUsingMeleeRange(
	AUnit* SeekingUnit,
	TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation
	)
{
	TArray<AActor*> ReturnedTargets;
	return ReturnedTargets;
}
