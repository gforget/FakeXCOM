// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericStack.h"
#include "UObject/Object.h"
#include "TilePathFinder.generated.h"

class AUnit;
class ATBTacticalGameMode;
class UNodePath;
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
	TArray<UNodePath*> AllNodePaths;
	
	UPROPERTY()
	bool bCanMoveUnit = true;
	
	void SubscribeOnUnitMovingEvents(UTileMovementComponent* UnitMovementComponent);
	void SubscribeGameModeEvent(ATBTacticalGameMode* TBTacticalGameMode);
	
	GenericStack<UNodePath*> GetPathToDestination(UNodePath* InitialNode, UNodePath* DestinationNode);

	void MoveUnit(const AUnit* Unit, GenericStack<UNodePath*> Path, bool CallEndOfAbility = true);

	void GetNodeDistanceLimitForUnit(AUnit* Unit,
		TArray<UNodePath*>& AllBaseDistanceNode,
		TArray<UNodePath*>& AllLongDistanceNode,
		int& BaseDistance,
		int& LongDistance);
	
private:

	UFUNCTION()
	void OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue);
	
	UFUNCTION()
	void OnUnitStartMovingEvent(AActor* MovingActor);

	UFUNCTION()
	void OnUnitStopMovingEvent(AActor* MovingActor);
	
	void AddNodeToCameFrom(TMap<int, UNodePath*>& came_from, int IdNode, UNodePath* ValueNode);
	UNodePath* GetNodeFromCameFrom(TMap<int, UNodePath*>& came_from, int IdNode);
	
	void AddCostToCostSoFar(TMap<int, float>& cost_so_far,int IdNode, float Cost);
	float GetCostFromCostSoFar(TMap<int, float>& cost_so_far, int IdNode);

	void AddStepToStepSoFar(TMap<int, int>& step_so_far,int IdNode, int Step);
	int GetStepFromStepSoFar(TMap<int, int>& step_so_far, int IdNode);
};
