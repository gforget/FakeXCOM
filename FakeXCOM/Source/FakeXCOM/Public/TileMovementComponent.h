// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericStack.h"
#include "Components/ActorComponent.h"
#include "TileMovementComponent.generated.h"

class UNodePath;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UTileMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTileMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	UNodePath* LocatedNodePath;

	UPROPERTY()
	FVector Destination;

	UPROPERTY()
	FVector MovementDelta;
	
	UPROPERTY()
	FVector MovementDirection;
	
	void FollowPath(const GenericStack<UNodePath*>& NewPath);
	
private:
	GenericStack<UNodePath*> Path;

	UPROPERTY()
	bool bChangeDestination;

	UPROPERTY()
	bool bStopMoving = true;
};
