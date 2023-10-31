// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMovementComponent.h"
#include "NodePath.h"

// Sets default values for this component's properties
UTileMovementComponent::UTileMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTileMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bStopMoving) return;
	
	if (bChangeDestination)
	{
		UNodePath* NodePathPtr = Path.Pop();
		if (Path.Size() == 0) LocatedNodePath = NodePathPtr;
		
		Destination = NodePathPtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f);
		MovementDirection = Destination - GetOwner()->GetActorLocation();
		MovementDirection.Normalize();
		bChangeDestination = false;
		
		DrawDebugSphere(GetWorld(), Destination, 10.0f, 12, FColor::Red, false, 5.0f, 0, 1.0f);
	}

	const FVector Delta = Destination - GetOwner()->GetActorLocation();
	DebugScreen(FString::Printf(TEXT("%f"), Delta.SizeSquared()), FColor::Green);
	
	if (Delta.SizeSquared() > 10.0f)
	{
		GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + MovementDirection*200.0f*DeltaTime);
	}
	else
	{
		if (Path.Size() > 0)
		{
			bChangeDestination = true;
		}
		else
		{
			bStopMoving = true;
		}
	}
}

void UTileMovementComponent::FollowPath(const GenericStack<UNodePath*>& NewPath)
{
	Path.Clear();
	Path = NewPath;
	bChangeDestination = true;
	bStopMoving = false;
}

