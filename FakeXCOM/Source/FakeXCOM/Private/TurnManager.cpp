﻿// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "TurnManager.h"
#include "FactionManager.h"
#include "TBTacticalGameMode.h"
#include "UnitAbilityManager.h"

// Sets default values for this component's properties
UTurnManager::UTurnManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	FactionTurnOrder.Add(EFaction::XCOM);
	FactionTurnOrder.Add(EFaction::Alien);
}

// Called when the game starts
void UTurnManager::BeginPlay()
{
	Super::BeginPlay();
	TBTacticalGameMode = Cast<ATBTacticalGameMode>(GetOwner());
}

EFaction UTurnManager::GetSelectedFaction()
{
	return FactionTurnOrder[SelectedFactionIndex];
}

void UTurnManager::EndTurn()
{
	if (TBTacticalGameMode->UnitAbilityManager->GetAbilitySelectionMode())
	{
		TBTacticalGameMode->UnitAbilityManager->DeactivateAbilitySelectionMode();
	}
	
	SelectedFactionIndex++;
	if (SelectedFactionIndex >= FactionTurnOrder.Num())
	{
		SelectedFactionIndex = 0;
	}
	
	OnTurnStartedEvent.Broadcast(GetSelectedFaction());
	TBTacticalGameMode->UnitManager->ResetAllActionsOfFaction(GetSelectedFaction());
	TBTacticalGameMode->UnitManager->SelectFirstUnitOfFaction(GetSelectedFaction());
	
}

