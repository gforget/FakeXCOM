// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"
#include "Gun.h"
#include "LevelBlock.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "TBTacticalMainController.h"
#include "TBTacticalGameMode.h"
#include "TileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AUnit::AUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetCapsuleRadius(34.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	
	RootComponent = CapsuleComponent;
	RootComponent->SetMobility(EComponentMobility::Movable);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TileMovementComponent = CreateDefaultSubobject<UTileMovementComponent>(TEXT("Tile Movement Component"));
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	
	if (GunClass)
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		Gun->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
		Gun->SetOwner(this);
	}
	else
	{
		DebugScreen("No Gun Set to this soldier", FColor::Red);
	}
}

void AUnit::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateHealthBarAnchorPositionVisualisation();	
}

void AUnit::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateHealthBarAnchorPositionVisualisation();
}

void AUnit::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateHealthBarAnchorPositionVisualisation();
}

void AUnit::Destroyed()
{
	Super::Destroyed();
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void AUnit::GenerateHealthBarAnchorPositionVisualisation() const
{
#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		UKismetSystemLibrary::FlushPersistentDebugLines(this);
		const FVector ActorLocation = GetActorLocation();
		DrawDebugSphere(GetWorld(), ActorLocation + HealthBarAnchor, 20.0f, 12, FColor::Cyan, true, 0.0f, 0, 0.0f);
	}
#endif
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUnit::Initialize()
{
	// Assign Node Reference
	
	// Define the line trace parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	
	// Perform the line trace
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = GetActorLocation() + FVector(0.0f,0.0f, -100.0f);

	if (bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		if (HitResult.GetActor())
		{
			if (ALevelBlock* LevelBlockPtr = Cast<ALevelBlock>(HitResult.GetActor()))
			{
				LevelBlockPtr->UnitOnBlock = this;
			
				TArray<UNodePath*> AllNodePaths;
				LevelBlockPtr->GetComponents<UNodePath>(AllNodePaths);
				UNodePath* StartingNodePtr = AllNodePaths[LevelBlockPtr->NodePathIndex];
				TileMovementComponent->LocatedNodePath = StartingNodePtr;
			}
		}
	}

	//Adding Reference to this soldier
	TBTacticalGameMode->AllUnitReference.Add(IdUnit, this);
	if (!TBTacticalGameMode->GetCurrentlySelectedUnit())
	{
		TBTacticalGameMode->SelectUnit(IdUnit);
	}
	
	TBTacticalGameMode->LevelUI->OnUnitSpawnEvent.Broadcast(this);
}

