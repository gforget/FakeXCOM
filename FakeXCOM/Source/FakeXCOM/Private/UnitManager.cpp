// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"
#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"
#include "UnitAttributeSet.h"

void UUnitManager::SelectNextUnit()
{
	//Soldier are never removed from the list, even when dead
	//TODO unit reference need to be seperated from ally and ennemy
	
	bool haveSelectedAUnit = false;
	for (int i=0; i<AllUnitReference.Num(); i++)
	{
		SelectedUnitId++;
		if (SelectedUnitId >= AllUnitReference.Num())
		{
			SelectedUnitId = 0;
		}

		if (AllUnitReference[SelectedUnitId]->UnitAttributeSet->GetActions() != 0)
		{
			haveSelectedAUnit = true;
			break;
		}
	}
	
	if (haveSelectedAUnit) SelectUnit(SelectedUnitId);
}

void UUnitManager::SelectPreviousUnit()
{
	bool haveSelectedAUnit = false;
	for (int i=0; i<AllUnitReference.Num(); i++)
	{
		SelectedUnitId--;
		if (SelectedUnitId < 0)
		{
			SelectedUnitId = AllUnitReference.Num()-1;
		}

		if (AllUnitReference[SelectedUnitId]->UnitAttributeSet->GetActions() != 0)
		{
			haveSelectedAUnit = true;
			break;
		}
	}
	
	if (haveSelectedAUnit) SelectUnit(SelectedUnitId);
}

AUnit* UUnitManager::SelectUnit(int UnitId, bool bGoToUnit)
{
	DebugScreen("New Soldier Selected !", FColor::Yellow);

	SelectedUnitId = UnitId;
	if (bGoToUnit)
	{
		TBTacticalGameMode->MainController->GoToActor(AllUnitReference[SelectedUnitId]);
	}
	OnUnitSelectedEvent.Broadcast(AllUnitReference[SelectedUnitId]);
	return AllUnitReference[SelectedUnitId];
}

AUnit* UUnitManager::GetCurrentlySelectedUnit()
{
	if (SelectedUnitId != -1)
	{
		return AllUnitReference[SelectedUnitId];	
	}
	
	return nullptr;
}

void UUnitManager::Initialize(ATBTacticalGameMode* TBTacticalGameModePtr)
{
	TBTacticalGameMode = TBTacticalGameModePtr;

	TBTacticalGameMode->MainController->MouseSceneSelectionComponent->OnLeftClickSelectActorEvent.AddDynamic(this, &UUnitManager::OnLeftClickSelectActor);
	TBTacticalGameMode->MainController->MouseSceneSelectionComponent->OnRightClickSelectActorEvent.AddDynamic(this, &UUnitManager::OnRightClickSelectActor);
}

void UUnitManager::AddUnitToManager(int IdUnit, AUnit* Unit)
{
	AllUnitReference.Add(IdUnit, Unit);
	Unit->OnUnitRanOutOfActionsEvent.AddDynamic(this, &UUnitManager::OnUnitRanOutOfActions);
}

void UUnitManager::OnUnitRanOutOfActions(AUnit* Unit)
{
	//TODO: seperate friendly unit and ennemy unit
	bAllUnitOutOfAction = true;
	for(int i=0; i<AllUnitReference.Num(); i++)
	{
		if (AllUnitReference[i]->UnitAttributeSet->GetActions() > 0)
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
		DebugScreen("End of turn", FColor::Red);
	}
	else
	{
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

void UUnitManager::OnRightClickSelectActor(AActor* Actor, FVector HitLocation)
{
	if (GetCurrentlySelectedUnit())
	{
		if (!TBTacticalGameMode->TilePathFinder->bCanMoveUnit)
		{
			return;
		}
		
		if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(Actor))
		{
			if (UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation))
			{
				if (!ChosenNodePath->bIsBlocked)
				{
					const AUnit* Unit = GetCurrentlySelectedUnit();
					const int BaseDistance = Unit->UnitAttributeSet->GetMaxMoveDistancePerAction();
					const int AllowedDistance = BaseDistance*Unit->UnitAttributeSet->GetActions();
					
					GenericStack<UNodePath*> Path = TBTacticalGameMode->TilePathFinder->GetPathToDestination(
						Unit->TileMovementComponent->LocatedNodePath,
						ChosenNodePath);
					
					if (Path.Num() > 0
						&& ChosenNodePath->NbSteps != -1
						&& ChosenNodePath->NbSteps <= AllowedDistance
						&& Unit->UnitAttributeSet->GetActions() > 0)
					{
						OnUnitOrderedToMoveEvent.Broadcast(GetCurrentlySelectedUnit());
						MovementActionCost(ChosenNodePath);
						TBTacticalGameMode->TilePathFinder->MoveUnit(GetCurrentlySelectedUnit(), Path);
					}
				}
			}
		}
	}
}

void UUnitManager::MovementActionCost(const UNodePath* Destination)
{
	const int BaseDistance = GetCurrentlySelectedUnit()->UnitAttributeSet->GetMaxMoveDistancePerAction();
	const int ActionCost = Destination->NbSteps > BaseDistance ? 2 : 1;
	GetCurrentlySelectedUnit()->UnitAttributeSet->SetActions(GetCurrentlySelectedUnit()->UnitAttributeSet->GetActions()-ActionCost);
}

void UUnitManager::OnLeftClickSelectActor(AActor* Actor, FVector HitLocation)
{
	if (const AUnit* SelectedUnitPtr = Cast<AUnit>(Actor))
	{
		SelectUnit(SelectedUnitPtr->IdUnit);
	}
}


