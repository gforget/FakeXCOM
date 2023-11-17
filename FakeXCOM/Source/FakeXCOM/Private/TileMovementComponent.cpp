// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMovementComponent.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "TilePathFinder.h"
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
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	if (TBTacticalGameMode)
	{
		TBTacticalGameMode->TilePathFinder->SubscribeOnUnitMovingEvents(this);
		TBTacticalGameMode->MainController->SubscribeOnUnitMovingEvents(this);
	}

	SetComponentTickEnabled(false);
}

// Called every frame
void UTileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bStopMoving)
	{
		CurrentVelocity = 0.0f;
		OnUnitStopMovingEvent.Broadcast(GetOwner());
		SetComponentTickEnabled(false);
		return;
	}
	
	if (bChangeDestination)
	{
		UNodePath* NodePathPtr = Path.Pop();
		if (Path.Size() == 0)
		{
			LocatedNodePath->bIsBlocked = false;
			LocatedNodePath = NodePathPtr;
			LocatedNodePath->bIsBlocked = true;
		} 
		
		Destination = NodePathPtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f);
		MovementDirection = Destination - GetOwner()->GetActorLocation();
		MovementDirection.Normalize();
		bChangeDestination = false;
	}
	
	//Rotate toward destination
	FVector ActorLocation = GetOwner()->GetActorLocation();
	ActorLocation.Z = 0.0f;
	
	FVector TargetLocation = ActorLocation + MovementDirection*MovementSpeed*DeltaTime;
	TargetLocation.Z = 0.0f;

	const FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
	const FRotator CurrentRotation = FMath::Lerp(GetOwner()->GetActorRotation(), NewRotation, RotationSpeed * DeltaTime);
	GetOwner()->SetActorRotation(CurrentRotation);
	
	//Move to destination
	const FVector Delta = Destination - GetOwner()->GetActorLocation();
	if (Delta.SizeSquared() > 10.0f)
	{
		const FVector DeltaDisplacement = MovementDirection*MovementSpeed*DeltaTime;
		CurrentVelocity = MovementSpeed;
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
			GetOwner()->SetActorLocation(Destination);
		}
	}
}

void UTileMovementComponent::FollowPath(const GenericStack<UNodePath*>& NewPath)
{
	Path.Clear();
	Path = NewPath;
	bChangeDestination = true;
	bStopMoving = false;
	OnUnitStartMovingEvent.Broadcast(GetOwner());
	SetComponentTickEnabled(true);
}

