// Fill out your copyright notice in the Description page of Project Settings.

#include "FakeXCOM/Public/LevelBlock.h"

#include "NodePath.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALevelBlock::ALevelBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ALevelBlock::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelBlock::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateNodePathVisualisation();
}

void ALevelBlock::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateNodePathVisualisation();
}

void ALevelBlock::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateNodePathVisualisation();
}

void ALevelBlock::Destroyed()
{
	Super::Destroyed();
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void ALevelBlock::GenerateNodePathVisualisation()
{
#if WITH_EDITOR
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	
	UWorld* World = GetWorld();
	float Radius = 20.0f;
	int32 Segments = 12;
	FColor Color = FColor::Cyan;
	float Duration = 0.0f; 
	float Thickness = 0.0f;
	FVector ActorLocation = GetActorLocation();
	for (int i=0; i<NodePathPositions.Num(); i++)
	{
		DrawDebugSphere(World, ActorLocation + NodePathPositions[i], Radius, Segments, Color, true, Duration, 0, Thickness);
	}
#endif
}

// Called every frame
void ALevelBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

