// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseSceneSelectionComponent.h"
#include "LevelBlock.h"
#include "NodePath.h"

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
}


// Called every frame
void UMouseSceneSelectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMouseSceneSelectionComponent::LeftClickSelection()
{
	FVector HitLocation;

	if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(SelectActorFromMousePosition(HitLocation)))
	{
		const UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation);
		
		UWorld* World = GetWorld();
		FVector Location = FVector(0, 0, 0); // Set the desired world location
		float Radius = 100.0f;
		int32 Segments = 12;
		FColor Color = FColor::Red;
		float Duration = 5.0f; // Will last for 5 seconds
		float Thickness = 1.0f;
		DrawDebugSphere(World, ChosenNodePath->GetComponentLocation(), Radius, Segments, Color, false, Duration, 0, Thickness);
	}
}

void UMouseSceneSelectionComponent::RightClickSelection()
{
	
}

AActor* UMouseSceneSelectionComponent::SelectActorFromMousePosition(FVector& HitPosition)
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
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Cyan, HitActorName);
				}
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Cyan, "No actor to return");
			}
		}
	}

	if (bDebugMouseLineTrace)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 10.0f, 0, 1);	
	}

	return ActorReturned;
}

