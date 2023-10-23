// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTacticalGameMode.h"
#include "TilePathFinder.h"

void ATBTacticalGameMode::BeginPlay()
{
	Super::BeginPlay();
	TilePathFinder = NewObject<UTilePathFinder>(GetTransientPackage(), UTilePathFinder::StaticClass());
}
