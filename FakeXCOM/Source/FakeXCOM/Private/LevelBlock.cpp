// Fill out your copyright notice in the Description page of Project Settings.

#include "FakeXCOM/Public/LevelBlock.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "TilePathFinder.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALevelBlock::ALevelBlock()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	RootComponent->SetMobility(EComponentMobility::Movable);
	
	// Create and initialize the ArrowComponent
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetVisibility(false);
	ArrowComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ALevelBlock::BeginPlay()
{
	Super::BeginPlay();
}
// Called every frame
void ALevelBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelBlock::PostActorCreated()
{
	Super::PostActorCreated();
	if (bIsStartingPosition && NodePathIndex != -1 && NodePathIndex < NodePathPositions.Num())
	{
		ArrowComponent->SetVisibility(true);
		FVector ArrowPosition = NodePathPositions[NodePathIndex];
		ArrowPosition.Z += 100.0f;
		ArrowComponent->SetRelativeLocation(ArrowPosition);
		ArrowComponent->SetRelativeRotation(FRotator(-90.0f,0.0f,0.0f));
	}
	else
	{
		ArrowComponent->SetVisibility(false);
	}

	GenerateNodePathPositionVisualisation();
}

void ALevelBlock::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (bIsStartingPosition && NodePathIndex != -1 && NodePathIndex < NodePathPositions.Num())
	{
		ArrowComponent->SetVisibility(true);
		FVector ArrowPosition = NodePathPositions[NodePathIndex];
		ArrowPosition.Z += 100.0f;
		ArrowComponent->SetRelativeLocation(ArrowPosition);
		ArrowComponent->SetRelativeRotation(FRotator(-90.0f,0.0f,0.0f));
	}
	else
	{
		ArrowComponent->SetVisibility(false);
	}
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
	
	FVector ActorLocation = GetActorLocation();
	for (int i=0; i<NodePathPositions.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), ActorLocation + NodePathPositions[i], 20.0f, 12, FColor::Cyan, true, 0.0f, 0, 0.0f);
	}

	if (IsSlope)
	{
		DrawDebugSphere(GetWorld(), ActorLocation + BottomSlopePosition, 20.0f, 12, FColor::Red, true, 0.0f, 0, 0.0f);
		DrawDebugSphere(GetWorld(), ActorLocation + TopSlopePosition, 20.0f, 12, FColor::Red, true, 0.0f, 0, 0.0f);
	}
#endif
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

