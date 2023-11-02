// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "TileMovementComponent.h"

// Sets default values
ASoldier::ASoldier()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TileMovementComponent = CreateDefaultSubobject<UTileMovementComponent>(TEXT("Tile Movement Component"));
}

// Called when the game starts or when spawned
void ASoldier::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

