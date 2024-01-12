// Fill out your copyright notice in the Description page of Project Settings.


#include "Consideration.h"

#include "Gun.h"
#include "GunAttributeSet.h"
#include "NodePath.h"
#include "TargetManager.h"
#include "TBTacticalGameMode.h"
#include "TileMovementComponent.h"

float UConsideration::ScoreConsideration_Implementation(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode)
{
	return 1.0f;
}

float UConsideration::ScoreConsiderationNode_Implementation(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode)
{
	return 1.0f;
}

float UConsideration::ScoreConsiderationActor_Implementation(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode,
	AActor* TargetActor)
{
	return 1.0f;
}

float UConsideration::GetLowestPotentialDefence(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode,
 UNodePath* TargetNode)
{
	
	// If there is no cover on that node, then the cover defence is 0
	if (TargetNode->AllCoverInfos.Num() == 0)
	{
		return 0.0f;	
	}
	
	// Get All Ennemy unit from enemy faction
	TArray<AUnit*> AllEnemyUnit = GetAllEnemyUnits(OwningUnit, TBTacticalGameMode);

	UTargetManager* TargetManagerRef = TBTacticalGameMode->TargetManager;
	float LowestPotentialCoverDefence = -1.0f; //-1 mean undefined, no one can have line of sight of that node
	
	for (int i=0; i<AllEnemyUnit.Num(); i++)
	{
		//Check if the ennemy is dead first
		if (AllEnemyUnit[i]->GetIsDead())
		{
			continue;
		}
		
		//Check if you have line of sight the unit if position at that node
		const float LineOfSightRange = AllEnemyUnit[i]->Gun->GunAttributeSet->GetRange();
		if (TargetManagerRef->ConfirmLineOfSightOnUnit(
			AllEnemyUnit[i],
			OwningUnit,
			AllEnemyUnit[i]->GetActorLocation(),
			TargetNode->GetComponentLocation() + FVector(0.0f, 0.0f, OwningUnit->ZGroundOffset),
			LineOfSightRange
			)
			)
		{
			//Get the cover value the unit will get for every cover in that position
			const float CurrentCoverDefense = AllEnemyUnit[i]->GetTargetCoverDefenceBonus(OwningUnit, AllEnemyUnit[i]->TileMovementComponent->LocatedNodePath,TargetNode);
			if (LowestPotentialCoverDefence > CurrentCoverDefense || LowestPotentialCoverDefence == -1.0f)
			{
				LowestPotentialCoverDefence = CurrentCoverDefense;
			}

			if (LowestPotentialCoverDefence == 0.0f)
			{
				break; //no point continuing, position is flanked
			}
		}		
	}

	//if the node has no line of sight, return the highest cover defence on it
	if (LowestPotentialCoverDefence == -1.0f)
	{
		LowestPotentialCoverDefence = 0.0f;
		for (int i=0; i<TargetNode->AllCoverInfos.Num(); i++)
		{
			float PotentialDefence = OwningUnit->FullCoverDefenceBonus;
			if (!TargetNode->AllCoverInfos[i].FullCover)
			{
				PotentialDefence = OwningUnit->LowCoverDefenceBonus;
			}
			
			if (LowestPotentialCoverDefence < PotentialDefence)
			{
				LowestPotentialCoverDefence = PotentialDefence;
			}
		}	
	}
	
	return LowestPotentialCoverDefence;
}

TArray<AUnit*> UConsideration::GetAllEnemyUnitInSight(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode,
	UNodePath* TargetNode)
{
	TArray<AUnit*> AllEnemyUnitInSight;
	
	// Get All Ennemy unit from enemy faction
	TArray<AUnit*> AllEnemyUnit = GetAllEnemyUnits(OwningUnit, TBTacticalGameMode);
	
	UTargetManager* TargetManagerRef = TBTacticalGameMode->TargetManager;
	const float LineOfSightRange = OwningUnit->Gun->GunAttributeSet->GetRange();
	
	for (int i=0; i<AllEnemyUnit.Num(); i++)
	{
		//Check if the ennemy is dead first
		if (AllEnemyUnit[i]->GetIsDead())
		{
			continue;
		}
		
		//Check if you have line of sight the unit if position at that node
		if (TargetManagerRef->ConfirmLineOfSightOnUnit(
			OwningUnit,
			AllEnemyUnit[i],
			TargetNode->GetComponentLocation() + FVector(0.0f, 0.0f, OwningUnit->ZGroundOffset),
			AllEnemyUnit[i]->GetActorLocation(),
			LineOfSightRange
			))
		{
			AllEnemyUnitInSight.Add(AllEnemyUnit[i]);
		}		
	}

	return AllEnemyUnitInSight;
}

TArray<AUnit*> UConsideration::GetAllEnemyUnits(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode)
{
	TArray<AUnit*> AllEnemyUnits;
	UFactionManager* FactionManagerRef = TBTacticalGameMode->FactionManagerComponent;
	const TMap<int, AUnit*> AllUnitMapRef = TBTacticalGameMode->UnitManager->AllUnitReference;
	
	for (const auto& Pair : AllUnitMapRef)
	{
		AUnit* OtherUnit = Pair.Value;
		if (FactionManagerRef->GetFactionRelation(OwningUnit->Faction, OtherUnit->Faction) == EFactionRelation::Enemy)
		{
			AllEnemyUnits.Add(OtherUnit);
		}
	}
	
	return AllEnemyUnits;
}
