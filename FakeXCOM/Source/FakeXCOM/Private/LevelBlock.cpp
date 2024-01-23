// Fill out your copyright notice in the Description page of Project Settings.

#include "FakeXCOM/Public/LevelBlock.h"
#include "NodePath.h"
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

#if WITH_EDITOR
void ALevelBlock::PostActorCreated()
{
	Super::PostActorCreated();
	
	ActivateArrowComponent();
	GenerateNodePathPositionVisualisation(GetWorld());
}

void ALevelBlock::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ActivateArrowComponent();
	GenerateNodePathPositionVisualisation(GetWorld());
}

void ALevelBlock::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateNodePathPositionVisualisation(GetWorld());
}
#endif

void ALevelBlock::Destroyed()
{
	Super::Destroyed();
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void ALevelBlock::GenerateNodePathPositionVisualisation(UWorld* World, bool bFlushDebugLine)
{
#if WITH_EDITOR
	if (World)
	{
		// if (GetWorld()->WorldType == EWorldType::EditorPreview)
		// {
			if (bFlushDebugLine)
			{
				UKismetSystemLibrary::FlushPersistentDebugLines(this);
			}
		
			const FVector ActorLocation = GetActorLocation();
			for (int i=0; i<NodePathPositions.Num(); i++)
			{
				DrawDebugSphere(World, ActorLocation + NodePathPositions[i], 20.0f, 12, FColor::Cyan, true, 0.0f, 0, 0.0f);
			}

			if (IsSlope)
			{
				DrawDebugSphere(World, ActorLocation + BottomSlopePosition, 20.0f, 12, FColor::Red, true, 0.0f, 0, 0.0f);
				DrawDebugSphere(World, ActorLocation + TopSlopePosition, 20.0f, 12, FColor::Red, true, 0.0f, 0, 0.0f);
			}
		//}
	}
#endif
}

void ALevelBlock::ActivateArrowComponent()
{
	if (bIsSpawningPosition)
	{
		for (int i=0; i<SpawningNodePathIndexes.Num(); i++)
		{
			if (SpawningNodePathIndexes[i] < NodePathPositions.Num())
			{
				ArrowComponent->SetVisibility(true);
				FVector ArrowPosition = NodePathPositions[SpawningNodePathIndexes[i]];
				ArrowPosition.Z += 100.0f;
				ArrowComponent->SetRelativeLocation(ArrowPosition);
				ArrowComponent->SetRelativeRotation(FRotator(-90.0f,0.0f,0.0f));
			}
		}
	}
	else
	{
		ArrowComponent->SetVisibility(false);
	}
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

