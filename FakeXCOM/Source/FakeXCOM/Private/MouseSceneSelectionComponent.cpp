// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseSceneSelectionComponent.h"

#include "DebugHeader.h"
#include "LevelBlock.h"
#include "NodePath.h"
#include "TBTacticalMainController.h"
#include "TBTacticalGameMode.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"
#include "UI3DManager.h"
#include "UnitManager.h"

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
	TBTacticalGameMode->MainController = Cast<ATBTacticalMainController>(GetOwner());
	SetComponentTickEnabled (false);
}

void UMouseSceneSelectionComponent::Initialize()
{
	TilePathFinder = TBTacticalGameMode->TilePathFinder;
	SetComponentTickEnabled (true);
}


// Called every frame
void UMouseSceneSelectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit() || !TilePathFinder->bCanMoveUnit) //Normally there is always a soldier selected
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
		
			TBTacticalGameMode->UI3DManagerComponent->ClearPath3DIcons();

			if (UTilePathFinder* TilePathFinderPtr = TBTacticalGameMode->TilePathFinder)
			{
				GenericStack<UNodePath*> PathStack = TilePathFinderPtr->GetPathToDestination(
						TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->TileMovementComponent->LocatedNodePath,
						CurrentMouseOverNodePath
						);
				
				while(!PathStack.IsEmpty())
				{
					const UNodePath* CurrentNodePath = PathStack.Pop();
					TBTacticalGameMode->UI3DManagerComponent->AddPath3DIcon(
					CurrentNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.6f),
					CurrentNodePath->GetComponentRotation()
					);
				}
			}
		
			//Create Select 3d Icon
			TBTacticalGameMode->UI3DManagerComponent->ClearSelect3DIcon();
			TBTacticalGameMode->UI3DManagerComponent->AddSelect3DIcon(
				CurrentMouseOverNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
				CurrentMouseOverNodePath->GetComponentRotation()
				);
			

			//Create Cover 3D Icon
			TBTacticalGameMode->UI3DManagerComponent->ClearCover3DIcons();
			for (int i=0; i<CurrentMouseOverNodePath->AllCoverInfos.Num(); i++)
			{
				TBTacticalGameMode->UI3DManagerComponent->AddCover3DIcon(
					CurrentMouseOverNodePath->AllCoverInfos[i].IconPosition,
					CurrentMouseOverNodePath->AllCoverInfos[i].IconRotation,
					CurrentMouseOverNodePath->AllCoverInfos[i].FullCover);
			}
		}
	}
}

void UMouseSceneSelectionComponent::LeftClickSelection()
{
	FVector HitLocation;
	if (const AUnit* SelectedUnitPtr = Cast<AUnit>(SelectActorFromMousePosition(HitLocation)))
	{
		TBTacticalGameMode->UnitManager->SelectUnit(SelectedUnitPtr->IdUnit);
	}
}

void UMouseSceneSelectionComponent::RightClickSelection()
{
	if (TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit())
	{
		if (!TilePathFinder->bCanMoveUnit)
		{
			return;
		}
		
		//clear the path icon
		TBTacticalGameMode->UI3DManagerComponent->ClearPath3DIcons();
		
		FVector HitLocation;
		if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(SelectActorFromMousePosition(HitLocation)))
		{
			if (UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation))
			{
				if (!ChosenNodePath->bIsBlocked)
				{
					TilePathFinder->MoveUnit(TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit(), ChosenNodePath);

					//Assign the cover icon so they do not get deleted when a unit is assign on a nodepath
					TBTacticalGameMode->UI3DManagerComponent->ConnectCover3DIconsToUnit(TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->IdUnit);
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

