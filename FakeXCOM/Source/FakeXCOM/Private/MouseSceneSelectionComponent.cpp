// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseSceneSelectionComponent.h"

#include "DebugHeader.h"
#include "LevelBlock.h"
#include "NodePath.h"
#include "Soldier.h"
#include "TBTacticalCameraController.h"
#include "TBTacticalGameMode.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"


// Sets default values for this component's properties
UMouseSceneSelectionComponent::UMouseSceneSelectionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMouseSceneSelectionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->InputComponent)
	{
		InputComponentPtr = GetOwner()->InputComponent;
		InputComponentPtr->BindAction("LeftClick", IE_Pressed, this, &UMouseSceneSelectionComponent::LeftClickSelection);
		InputComponentPtr->BindAction("RightClick", IE_Pressed, this, &UMouseSceneSelectionComponent::RightClickSelection);
	}
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	TBTacticalGameMode->CameraController = Cast<ATBTacticalCameraController>(GetOwner());
	TilePathFinder = TBTacticalGameMode->TilePathFinder;
}


// Called every frame
void UMouseSceneSelectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//TODO: Could create an event so we know when GameMode has finish it begin play
	if (!TilePathFinder)
	{
		TilePathFinder = TBTacticalGameMode->TilePathFinder;
	}
	
	if (!SelectedSoldier || !TilePathFinder->bCanMoveUnit) //Normally there is always a soldier selected
	{
		return;
	}
	
	FVector HitLocation;
	if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(SelectActorFromMousePosition(HitLocation)))
	{
		UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation);
		if (ChosenNodePath && (!CurrentMouseOverNodePath || CurrentMouseOverNodePath->IdNode != ChosenNodePath->IdNode))
		{
			CurrentMouseOverNodePath = ChosenNodePath;
			
			//Create Path3dIcons
			if (Path3DIconClass)
			{
				if (AllPath3DIcons.Num() > 0)
				{
					for (int i=0; i<AllPath3DIcons.Num();i++)
					{
						AllPath3DIcons[i]->Destroy();
					}
					AllPath3DIcons.Empty();
				}

				if (UTilePathFinder* TilePathFinderPtr = TBTacticalGameMode->TilePathFinder)
				{
					GenericStack<UNodePath*> PathStack = TilePathFinderPtr->GetPathToDestination(
							SelectedSoldier->TileMovementComponent->LocatedNodePath,
							CurrentMouseOverNodePath
							);
					
					while(!PathStack.IsEmpty())
					{
						const UNodePath* CurrentNodePath = PathStack.Pop();
						AllPath3DIcons.Add(GetWorld()->SpawnActor<AActor>(Path3DIconClass,
							CurrentNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
							CurrentNodePath->GetComponentRotation()
							));
					}
				}
			}
			
			//Create Select 3d Icon
			if (Select3DIconClass)
			{
				if (Select3DIcon)
				{
					Select3DIcon->Destroy();
					Select3DIcon = nullptr;
				}
				
				Select3DIcon = GetWorld()->SpawnActor<AActor>(Select3DIconClass,
					CurrentMouseOverNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
					CurrentMouseOverNodePath->GetComponentRotation()
					);
			}

			//Create Cover 3D Icon
			if (Cover3DIconClass)
			{
				if (AllMouseOverCover3DIcon.Num() > 0)
				{
					for (int i=0; i<AllMouseOverCover3DIcon.Num(); i++)
					{
						AllMouseOverCover3DIcon[i]->Destroy();
					}
					AllMouseOverCover3DIcon.Empty();
				}
				
				for (int i=0; i<CurrentMouseOverNodePath->AllCoverInfos.Num(); i++)
				{
					ACover3DIcon* Cover3DIcon = GetWorld()->SpawnActor<ACover3DIcon>(Cover3DIconClass,
						CurrentMouseOverNodePath->AllCoverInfos[i].IconPosition,
						CurrentMouseOverNodePath->AllCoverInfos[i].IconRotation);

					if (CurrentMouseOverNodePath->AllCoverInfos[i].FullCover)
					{
						Cover3DIcon->FullShield->SetVisibility(true);
					}
					else
					{
						Cover3DIcon->HalfShield->SetVisibility(true);
					}
					
					AllMouseOverCover3DIcon.Add(Cover3DIcon);
				}
			}
		}
	}
}

void UMouseSceneSelectionComponent::LeftClickSelection()
{
	FVector HitLocation;
	if (ASoldier* SelectedSoldierPtr = Cast<ASoldier>(SelectActorFromMousePosition(HitLocation)))
	{
		SelectedSoldier = SelectedSoldierPtr;
		DebugScreen("New Soldier Selected !", FColor::Yellow);
	}
}

void UMouseSceneSelectionComponent::RightClickSelection()
{
	if (SelectedSoldier)
	{
		if (!TilePathFinder->bCanMoveUnit)
		{
			return;
		}
		
		//clear the path icon
		if (AllPath3DIcons.Num() > 0)
		{
			for (int i=0; i<AllPath3DIcons.Num();i++)
			{
				AllPath3DIcons[i]->Destroy();
			}
			AllPath3DIcons.Empty();
		}
		
		FVector HitLocation;
		if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(SelectActorFromMousePosition(HitLocation)))
		{
			if (UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation))
			{
				const ALevelBlock* LevelBlockPtr = Cast<ALevelBlock>(ChosenNodePath->GetOwner());
				if (LevelBlockPtr->UnitOnBlock == nullptr)
				{
					TilePathFinder->MoveUnit(SelectedSoldier, ChosenNodePath);

					//Assign the cover icon so they do not get deleted when a unit is assign on a nodepath
					if (!AllAssignCover3DIcon.Contains(SelectedSoldier->IdUnit))
					{
						AllAssignCover3DIcon.Add(SelectedSoldier->IdUnit, FAssignCover3DIcon());
					}
					
					if (AllAssignCover3DIcon[SelectedSoldier->IdUnit].Entries.Num() > 0)
					{
						for (int i=0; i<AllAssignCover3DIcon[SelectedSoldier->IdUnit].Entries.Num(); i++)
						{
							AllAssignCover3DIcon[SelectedSoldier->IdUnit].Entries[i]->Destroy();
						}
						AllAssignCover3DIcon[SelectedSoldier->IdUnit].Entries.Empty();
					}
					
					AllAssignCover3DIcon[SelectedSoldier->IdUnit].Entries.Append(AllMouseOverCover3DIcon);
					AllMouseOverCover3DIcon.Empty();
				}
			}
		}
	}
}

AActor* UMouseSceneSelectionComponent::SelectActorFromMousePosition(FVector& HitPosition, bool bDebugShowActorNameReturned) const
{
	AActor* ActorReturned = nullptr;
	
	// Get the player controller
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return nullptr;
	}
	
	// Get the camera location and direction
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Calculate the end location for the line trace
	FVector WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(CameraLocation, WorldDirection);

	// Define the line trace parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;

	// Perform the line trace
	FVector StartLocation = CameraLocation;
	FVector EndLocation = CameraLocation + WorldDirection * 10000.0f;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

	// Check if something was hit
	if (bHit)
	{
		// Handle the hit result (e.g., print the hit actor's name)
		if (HitResult.GetActor())
		{
			ActorReturned = HitResult.GetActor();
			HitPosition = HitResult.Location;
			
			if (bDebugShowActorNameReturned)
			{
				FString HitActorName = HitResult.GetActor()->GetName();
				DebugScreen(HitActorName, FColor::Cyan);
			}
		}
		else
		{
			DebugScreen("No actor to return", FColor::Cyan);
		}
	}

	if (bDebugMouseLineTrace)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 10.0f, 0, 1);	
	}

	return ActorReturned;
}

