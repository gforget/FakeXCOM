﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"

#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"
#include "TurnManager.h"
#include "UnitAbility.h"
#include "UnitAbilityManager.h"
#include "UnitAttributeSet.h"

void UUnitManager::SelectNextUnit()
{
	bool haveSelectedAUnit = false;
	int UnitID = -1;
	
	const EFaction SelectedFaction = TBTacticalGameMode->TurnManagerComponent->GetSelectedFaction();
	for (int i=0; i<AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction.Num(); i++)
	{
		SelectedUnitIndex++;
		if (SelectedUnitIndex >= AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction.Num())
		{
			SelectedUnitIndex = 0;
		}

		UnitID = AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction[SelectedUnitIndex];
		AUnit* SelectedUnit = AllUnitReference[UnitID];
		
		if (SelectedUnit->UnitAttributeSet->GetActions() != 0 && !SelectedUnit->GetIsDead())
		{
			haveSelectedAUnit = true;
			break;
		}
	}
	
	if (haveSelectedAUnit && UnitID != -1)
	{
		SelectUnit(UnitID);
	}
}

void UUnitManager::SelectPreviousUnit()
{
	bool haveSelectedAUnit = false;
	int UnitID = -1;
	const EFaction SelectedFaction = TBTacticalGameMode->TurnManagerComponent->GetSelectedFaction();
	
	for (int i=0; i<AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction.Num(); i++)
	{
		SelectedUnitIndex--;
		if (SelectedUnitIndex < 0)
		{
			SelectedUnitIndex = AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction.Num()-1;
		}

		UnitID = AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction[SelectedUnitIndex];
		AUnit* SelectedUnit = AllUnitReference[UnitID];
		
		if (SelectedUnit->UnitAttributeSet->GetActions() != 0 && !SelectedUnit->GetIsDead())
		{
			haveSelectedAUnit = true;
			break;
		}
	}
	
	if (haveSelectedAUnit && UnitID != -1)
	{
		SelectUnit(UnitID);
	}
}

AUnit* UUnitManager::SelectUnit(int UnitId, bool bGoToUnit)
{
	DebugScreen("New Soldier Selected !", FColor::Yellow);
	
	AUnit* SelectedUnit = AllUnitReference[UnitId];
	const EFaction SelectedFaction = TBTacticalGameMode->TurnManagerComponent->GetSelectedFaction();

	if (SelectedUnit->GetIsDead())
	{
		return nullptr;
	}
	
	if (SelectedUnit->Faction != SelectedFaction)
	{
		//TODO: could select an enemy if the unit is on sight
		return nullptr;
	}
	
	for (int i=0; i<AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction.Num(); i++)
	{
		if (AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction[i] == UnitId)
		{
			SelectedUnitIndex = i;
		}
	}
	
	if (bGoToUnit)
	{
		TBTacticalGameMode->MainController->GoToActor(SelectedUnit);
	}
	
	//the ability cannot subscribe to the event, have to manually call the event
	for(int i=0; i<SelectedUnit->OwnedAbilities.Num(); i++)
	{
		SelectedUnit->OwnedAbilities[i]->OnUnitSelected(SelectedUnit->IdUnit);
	}

	OnUnitSelectedEvent.Broadcast(SelectedUnit);
	return SelectedUnit;
}

AUnit* UUnitManager::GetUnitFromId(int UnitId)
{
	return AllUnitReference[UnitId];
}

AUnit* UUnitManager::GetCurrentlySelectedUnit()
{
	if (SelectedUnitIndex != -1)
	{
		const EFaction SelectedFaction = TBTacticalGameMode->TurnManagerComponent->GetSelectedFaction();
		const int UnitId = AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction[SelectedUnitIndex];
		return AllUnitReference[UnitId];	
	}
	
	return nullptr;
}

void UUnitManager::Initialize(ATBTacticalGameMode* TBTacticalGameModePtr)
{
	TBTacticalGameMode = TBTacticalGameModePtr;

	TBTacticalGameMode->MainController->MouseSceneSelectionComponent->OnLeftClickSelectActorEvent.AddDynamic(this, &UUnitManager::OnLeftClickSelectActor);
	TBTacticalGameMode->MainController->MouseSceneSelectionComponent->OnRightClickSelectActorEvent.AddDynamic(this, &UUnitManager::OnRightClickSelectActor);
}

TArray<int> UUnitManager::GetAllUnitsIdFromFactions(EFaction Faction)
{
	TArray<int> AllUnitsIdFromFaction;
	if (!AllUnitFactionReferenceMap.Contains(Faction))
	{
		AllUnitFactionReferenceMap.Add(Faction, FUnitFactionStruct());
	}
	
	for (int i=0; i<AllUnitFactionReferenceMap[Faction].UnitInFaction.Num(); i++)
	{
		AllUnitsIdFromFaction.Add(AllUnitFactionReferenceMap[Faction].UnitInFaction[i]);
	}

	return AllUnitsIdFromFaction;
}

void UUnitManager::AddUnitToManager(int IdUnit, AUnit* Unit)
{
	if (!AllUnitFactionReferenceMap.Contains(Unit->Faction))
	{
		AllUnitFactionReferenceMap.Add(Unit->Faction, FUnitFactionStruct());
	}
	
	AllUnitFactionReferenceMap[Unit->Faction].UnitInFaction.Add(IdUnit);
	AllUnitReference.Add(IdUnit, Unit);

	OnUnitSpawnedEvent.Broadcast(Unit);
	Unit->OnUnitRanOutOfActionsEvent.AddDynamic(this, &UUnitManager::OnUnitRanOutOfActions);
}

void UUnitManager::OnUnitRanOutOfActions(AUnit* Unit)
{
	bAllUnitOutOfAction = true;
	const EFaction SelectedFaction = TBTacticalGameMode->TurnManagerComponent->GetSelectedFaction();
	
	for(int i=0; i<AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction.Num(); i++)
	{
		const int UnitId = AllUnitFactionReferenceMap[SelectedFaction].UnitInFaction[i];

		AUnit* UnitPtr = AllUnitReference[UnitId];

		if (UnitPtr->GetIsDead())
		{
			continue;	
		}
		
		if (UnitPtr->UnitAttributeSet->GetActions() > 0)
		{
			bAllUnitOutOfAction = false;
			break;
		}
	}
}

void UUnitManager::EndOfAbility()
{
	if (bAllUnitOutOfAction)
	{
		//End the turn
		TBTacticalGameMode->TurnManagerComponent->EndTurn();
	}
	else
	{
		if (TBTacticalGameMode->UnitAbilityManager->GetAbilitySelectionMode())
		{
			TBTacticalGameMode->UnitAbilityManager->DeactivateAbilitySelectionMode();
		}
		
		if (GetCurrentlySelectedUnit()->UnitAttributeSet->GetActions() > 0)
		{
			SelectUnit(GetCurrentlySelectedUnit()->IdUnit, false);
		}
		else
		{
			SelectNextUnit();
		}
	}
}

void UUnitManager::SelectFirstUnitOfFaction(EFaction Faction)
{
	if (AllUnitFactionReferenceMap[Faction].UnitInFaction.Num() > 0)
	{
		SelectedUnitIndex = 0;
		
		bool AllUnitInFactionAreDead = false;
		while (!AllUnitInFactionAreDead && GetCurrentlySelectedUnit()->GetIsDead())
		{
			SelectedUnitIndex++;
			if (SelectedUnitIndex >= AllUnitFactionReferenceMap[Faction].UnitInFaction.Num())
			{
				AllUnitInFactionAreDead = true;
				SelectedUnitIndex = -1;
			}
		}
		
		if (!AllUnitInFactionAreDead)
		{
			SelectUnit(GetCurrentlySelectedUnit()->IdUnit);
		}
	}
}

void UUnitManager::ResetAllActionsOfFaction(EFaction Faction)
{
	bAllUnitOutOfAction = false;
	for (int i=0; i<AllUnitFactionReferenceMap[Faction].UnitInFaction.Num(); i++)
	{
		AUnit* UnitPtr = AllUnitReference[AllUnitFactionReferenceMap[Faction].UnitInFaction[i]];
		if (UnitPtr->GetIsDead())
		{
			continue;	
		}
		UnitPtr->UnitAttributeSet->SetActions(UnitPtr->UnitAttributeSet->GetMaxActions());
	}
}

void UUnitManager::OnRightClickSelectActor(AActor* Actor, FVector HitLocation)
{
	if (GetCurrentlySelectedUnit())
	{
		if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(Actor))
		{
			if (UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation))
			{
				GetCurrentlySelectedUnit()->MoveToNodePath(ChosenNodePath);
			}
		}
	}
}

void UUnitManager::OnLeftClickSelectActor(AActor* Actor, FVector HitLocation)
{
	if(TBTacticalGameMode->UnitAbilityManager->GetAbilitySelectionMode())
	{
		//TODO: modify this part so you can select target from the 3D scene depending on the faction target
		return;	
	}
	
	if (const AUnit* SelectedUnitPtr = Cast<AUnit>(Actor))
	{
		SelectUnit(SelectedUnitPtr->IdUnit);
	}
}



