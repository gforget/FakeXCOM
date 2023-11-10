// Fill out your copyright notice in the Description page of Project Settings.

#include "Soldier.h"
#include "Gun.h"
#include "LevelBlock.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "TBTacticalCameraController.h"
#include "TBTacticalGameMode.h"
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
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	
	if (GunClass)
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
		Gun->SetOwner(this);
	}
	else
	{
		DebugScreen("No Gun Set to this soldier", FColor::Red);
	}

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
	
	if (!TBTacticalGameMode->CameraController->MouseSceneSelectionComponent->SelectedSoldier)
	{
		TBTacticalGameMode->CameraController->MouseSceneSelectionComponent->SelectedSoldier = this;
	}
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

