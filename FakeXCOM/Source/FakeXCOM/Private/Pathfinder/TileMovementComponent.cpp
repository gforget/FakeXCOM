// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "Pathfinder/TileMovementComponent.h"
#include "TBTacticalGameMode.h"

#include "Controller/TBTacticalMainController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Managers/UnitManager.h"
#include "Pathfinder/NodePath.h"
#include "Pathfinder/TilePathFinder.h"

// Sets default values for this component's properties
UTileMovementComponent::UTileMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTileMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningUnit = Cast<AUnit>(GetOwner());
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	if (TBTacticalGameMode)
	{
		TBTacticalGameMode->TilePathFinder->SubscribeOnUnitMovingEvents(this);
		TBTacticalGameMode->MainController->SubscribeOnUnitMovingEvents(this);
	}

	if (OwningUnit)
	{
		OwningUnit->OnUnitIsDeadEvent.AddDynamic(this, &UTileMovementComponent::OnUnitIsDead);
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
		if (bCallEndOfAbility)
		{
			TBTacticalGameMode->UnitManager->EndOfAbility();
		}
		
		SetComponentTickEnabled(false);
		return;
	}
	
	if (bChangeDestination)
	{
		UNodePath* NodePathPtr = Path.Pop();
		if (Path.Num() == 0)
		{
			LocatedNodePath->bIsBlocked = false;
			LocatedNodePath = NodePathPtr;
			LocatedNodePath->bIsBlocked = true;
		} 
		
		Destination = NodePathPtr->GetComponentLocation() + FVector(0.0f,0.0f ,OwningUnit->ZGroundOffset);
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
	const float DeltaSize = Delta.SizeSquared();
	
	if (DeltaSize < DeltaMem || DeltaMem == -1.0f) 
	{
		const FVector DeltaDisplacement = MovementDirection*MovementSpeed*DeltaTime;
		CurrentVelocity = MovementSpeed;
		GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + DeltaDisplacement);
		DeltaMem = Delta.SizeSquared();
	}
	else
	{
		if (Path.Num() > 0)
		{
			bChangeDestination = true;
		}
		else
		{
			bStopMoving = true;
			GetOwner()->SetActorLocation(Destination);
		}
		DeltaMem = -1;
	}
}

void UTileMovementComponent::FollowPath(const GenericStack<UNodePath*>& NewPath, bool CallEndOfAbility)
{
	Path.Clear();
	Path = NewPath;
	bCallEndOfAbility = CallEndOfAbility;
	bChangeDestination = true;
	bStopMoving = false;
	OnUnitStartMovingEvent.Broadcast(GetOwner());
	SetComponentTickEnabled(true);
}

void UTileMovementComponent::OnUnitIsDead(AUnit* DeadUnit)
{
	Path.Clear();
	bStopMoving = true;
	LocatedNodePath->bIsBlocked = false;
	LocatedNodePath = nullptr;
}


