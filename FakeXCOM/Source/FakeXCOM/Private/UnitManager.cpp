// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"
#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "TilePathFinder.h"
#include "UI3DManager.h"

void UUnitManager::SelectNextUnit()
{
	//TODO: create a loop to select soldier who turn didn't finish and can perform action this turn
	//Soldier are never removed from the list, even when dead
	
	SelectedUnitId++;
	if (SelectedUnitId >= AllUnitReference.Num())
	{
		SelectedUnitId = 0;
	}

	SelectUnit(SelectedUnitId);
}

void UUnitManager::SelectPreviousUnit()
{
	SelectedUnitId--;
	if (SelectedUnitId < 0)
	{
		SelectedUnitId = AllUnitReference.Num()-1;
	}
	SelectUnit(SelectedUnitId);
}

AUnit* UUnitManager::SelectUnit(int UnitId)
{
	DebugScreen("New Soldier Selected !", FColor::Yellow);

	SelectedUnitId = UnitId;
	TBTacticalGameMode->MainController->GoToActor(AllUnitReference[SelectedUnitId]);
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

void UUnitManager::OnRightClickSelectActor(AActor* Actor, FVector HitLocation)
{
	if (GetCurrentlySelectedUnit())
	{
		if (!TBTacticalGameMode->TilePathFinder->bCanMoveUnit)
		{
			return;
		}
		
		//clear the path icon
		//TBTacticalGameMode->UI3DManagerComponent->ClearPath3DIcons();
		
		if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(Actor))
		{
			if (UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation))
			{
				if (!ChosenNodePath->bIsBlocked)
				{
					OnUnitOrderedToMoveEvent.Broadcast(GetCurrentlySelectedUnit());
					TBTacticalGameMode->TilePathFinder->MoveUnit(GetCurrentlySelectedUnit(), ChosenNodePath);
					
					//Assign the cover icon so they do not get deleted when a unit is assign on a nodepath
					//TBTacticalGameMode->UI3DManagerComponent->ConnectCover3DIconsToUnit(TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->IdUnit);
				}
			}
		}
	}
}

void UUnitManager::OnLeftClickSelectActor(AActor* Actor, FVector HitLocation)
{
	if (const AUnit* SelectedUnitPtr = Cast<AUnit>(Actor))
	{
		SelectUnit(SelectedUnitPtr->IdUnit);
	}
}
