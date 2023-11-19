// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"

#include "DebugHeader.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"

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
}
