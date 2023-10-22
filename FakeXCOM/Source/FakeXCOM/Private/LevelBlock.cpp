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
	GenerateNodePathPositionVisualisation();
}

void ALevelBlock::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateNodePathPositionVisualisation();
}

void ALevelBlock::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateNodePathPositionVisualisation();
}

void ALevelBlock::Destroyed()
{
	Super::Destroyed();
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void ALevelBlock::GenerateNodePathPositionVisualisation()
{
#if WITH_EDITOR
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	
	UWorld* World = GetWorld();
	float Radius = 20.0f;
	int32 Segments = 12;
	float Duration = 0.0f; 
	float Thickness = 0.0f;
	FVector ActorLocation = GetActorLocation();
	
	for (int i=0; i<NodePathPositions.Num(); i++)
	{
		DrawDebugSphere(World, ActorLocation + NodePathPositions[i], Radius, Segments, FColor::Cyan, true, Duration, 0, Thickness);
	}

	if (IsSlope)
	{
		DrawDebugSphere(World, ActorLocation + BottomSlopePosition, Radius, Segments, FColor::Red, true, Duration, 0, Thickness);
		DrawDebugSphere(World, ActorLocation + TopSlopePosition, Radius, Segments, FColor::Red, true, Duration, 0, Thickness);
	}
#endif
}

// Called every frame
void ALevelBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UNodePath* ALevelBlock::GetClosestNodePathFromLocation(FVector Location)
{
	UNodePath* ChosenNodePath = nullptr;
	
	TArray<UNodePath*> AllNodePaths;
	GetComponents<UNodePath>(AllNodePaths);
	if (AllNodePaths.Num() > 0)
	{
		// Get the closest node path to the hit, this will be the targeted neighbour
		TArray<UNodePath*> SortedNodePaths = AllNodePaths;
					
		SortedNodePaths.Sort([Location](const UNodePath& A, const UNodePath& B) {
			return FVector::DistSquared2D(A.GetComponentLocation(), Location) < FVector::DistSquared2D(B.GetComponentLocation(), Location);
		});
			
		ChosenNodePath = SortedNodePaths[0];
	}

	return ChosenNodePath;
}

