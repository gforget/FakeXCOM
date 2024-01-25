// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "MouseSceneSelectionComponent.h"

#include "DebugHeader.h"
#include "LevelBlock.h"
#include "TBTacticalMainController.h"
#include "TBTacticalGameMode.h"
#include "TilePathFinder.h"
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

	if (TBTacticalGameMode->FactionManagerComponent->FactionsController[TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->Faction]
		== EAssignedController::AIController)
	{
		return;
	}
	
	FVector HitLocation;
	if (AActor* ReturnedActor = SelectActorFromMousePosition(HitLocation))
	{
		if (!CurrentMouseOverActor)
		{
			CurrentMouseOverActor = ReturnedActor;
			OnMouseOverActorEvent.Broadcast(ReturnedActor, HitLocation);
		}
		
		//if (ReturnedActor->GetActorGuid() != CurrentMouseOverActor->GetActorGuid()) //only work in editor, might want to find an altnernative
		if (ReturnedActor->GetName() != CurrentMouseOverActor->GetName())
		{
			OnMouseOverActorEvent.Broadcast(ReturnedActor, HitLocation);
		}	
	}
}

void UMouseSceneSelectionComponent::LeftClickSelection()
{
	if (TBTacticalGameMode->FactionManagerComponent->FactionsController[TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->Faction]
	== EAssignedController::AIController)
	{
		return;
	}
	FVector HitLocation;
	AActor* ReturnedActor = SelectActorFromMousePosition(HitLocation);
	OnLeftClickSelectActorEvent.Broadcast(ReturnedActor, HitLocation);
}

void UMouseSceneSelectionComponent::RightClickSelection()
{
	if (TBTacticalGameMode->FactionManagerComponent->FactionsController[TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->Faction]
	== EAssignedController::AIController)
	{
		return;
	}
	
	FVector HitLocation;
	AActor* ReturnedActor = SelectActorFromMousePosition(HitLocation);
	OnRightClickSelectActorEvent.Broadcast(ReturnedActor, HitLocation);
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

