// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionManager.h"


// Sets default values for this component's properties
UFactionManager::UFactionManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	FactionsController.Add(EFaction::Player, EAssignedController::PlayerController);
	FactionsController.Add(EFaction::Enemy, EAssignedController::AIController);
	FactionsController.Add(EFaction::Ally, EAssignedController::AIController);
	FactionsController.Add(EFaction::Neutral, EAssignedController::AIController);
}

void UFactionManager::BeginPlay()
{
	Super::BeginPlay();
}

