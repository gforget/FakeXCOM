// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericStack.h"
#include "Components/ActorComponent.h"
#include "TileMovementComponent.generated.h"

class UNodePath;
class ATBTacticalGameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitStartMovingDelegate, AActor*, ActorMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitStopMovingDelegate, AActor*, ActorMoving);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Properties")
	float MovementSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Properties")
	float RotationSpeed = 25.0f;
	
	UPROPERTY()
	float CurrentVelocity = 0.0f;
	
	UPROPERTY()
	UNodePath* LocatedNodePath;

	UPROPERTY()
	FVector Destination;

	UPROPERTY()
	FVector MovementDelta;
	
	UPROPERTY()
	FVector MovementDirection;

	UPROPERTY(BlueprintAssignable)
	FUnitStartMovingDelegate OnUnitStartMovingEvent;
	
	UPROPERTY(BlueprintAssignable)
	FUnitStopMovingDelegate OnUnitStopMovingEvent;
	
	void FollowPath(const GenericStack<UNodePath*>& NewPath, bool CallEndOfAbility);

private:
	GenericStack<UNodePath*> Path;

	UPROPERTY()
	bool bCallEndOfAbility = false;
	
	UPROPERTY()
	bool bChangeDestination = false;

	UPROPERTY()
	bool bStopMoving = true;

	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
};
