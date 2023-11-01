// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMovementComponent.h"
#include "NodePath.h"
#include "Kismet/KismetMathLibrary.h"

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
	if (bStopMoving)
	{
		CurrentVelocity = 0.0f;
		return;
	}
	
	if (bChangeDestination)
	{
		UNodePath* NodePathPtr = Path.Pop();
		if (Path.Size() == 0) LocatedNodePath = NodePathPtr;
		
		Destination = NodePathPtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f);
		MovementDirection = Destination - GetOwner()->GetActorLocation();
		MovementDirection.Normalize();
		bChangeDestination = false;
	}
	
	//Rotate toward destination
	FVector TargetLocation = Destination;
	TargetLocation.Z = 0.0f;
	
	FVector ActorLocation = GetOwner()->GetActorLocation();
	ActorLocation.Z = 0.0f;
	
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
	
	float InterpSpeed = RotationSpeed;
	FRotator CurrentRotation = FMath::Lerp(GetOwner()->GetActorRotation(), NewRotation, InterpSpeed * DeltaTime);

	GetOwner()->SetActorRotation(CurrentRotation);
	
	//Move to destination
	const FVector Delta = Destination - GetOwner()->GetActorLocation();
	if (Delta.SizeSquared() > 10.0f)
	{
		const FVector DeltaDisplacement = MovementDirection*MovementSpeed*DeltaTime;
		CurrentVelocity = DeltaDisplacement.Size();
		GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + DeltaDisplacement);
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

