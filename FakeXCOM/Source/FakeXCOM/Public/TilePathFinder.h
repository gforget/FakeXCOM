// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericStack.h"
#include "UObject/Object.h"
#include "TilePathFinder.generated.h"

class UNodePath;
class AUnit;
class UTileMovementComponent;
/**
 * 
 */
UCLASS()
class FAKEXCOM_API UTilePathFinder : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bCanMoveUnit = true;

	UFUNCTION()
	void OnUnitStartMovingEvent(AActor* MovingActor);

	UFUNCTION()
	void OnUnitStopMovingEvent(AActor* MovingActor);
	
	void SubscribeOnUnitMovingEvents(UTileMovementComponent* UnitMovementComponent);
	
	GenericStack<UNodePath*> GetPathToDestination(UNodePath* InitialNode, UNodePath* DestinationNode);

	void MoveUnit(const AUnit* Unit, UNodePath* ChosenNode);

	void GetNodeDistanceLimitForUnit(AUnit* Unit, TArray<UNodePath*>& BaseDistance, TArray<UNodePath*>& LongDistance);
	
private:
	void AddNodeToCameFrom(TMap<int, UNodePath*>& came_from, int IdNode, UNodePath* ValueNode);
	UNodePath* GetNodeFromCameFrom(TMap<int, UNodePath*>& came_from, int IdNode);
	
	void AddCostToCostSoFar(TMap<int, float>& cost_so_far,int IdNode, float Cost);
	float GetCostFromCostSoFar(TMap<int, float>& cost_so_far, int IdNode);
};
